'''
The content of this file could be self-defined. 
But please note the interface of the following function cannot be modified,
    - encFunction_1
    - decFunction_1
    - encFunction_2
    - decFunction_2
'''
# =======================================================================================================================
# =======================================================================================================================
# Package Importing
import time
import numpy as np
from numpy import ndarray
import os
from typing import Dict
import scipy.io as sc
import torch
import os
from torch import nn
from torch.utils.data import Dataset
# from strategy.dataset_loader import DatasetLoader
from collections import OrderedDict


# =======================================================================================================================
# =======================================================================================================================
# Number to Bit Function Defining
def Num2Bit(num, B):
    num_ = num.type(torch.uint8)

    def integer2bit(integer, num_bits=B * 2):
        dtype_ = integer.type()
        exponent_bits = -torch.arange(-(num_bits - 1), 1).type(dtype_)
        exponent_bits = exponent_bits.repeat(integer.shape + (1,))
        out = integer.unsqueeze(-1) // 2 ** exponent_bits
        return (out - (out % 1)) % 2

    bit = integer2bit(num_)
    bit = (bit[:, :, B:]).reshape(-1, num_.shape[1] * B)
    return bit.type(torch.float32)


def Bit2Num(bit, B):
    bit_ = bit.type(torch.float32)
    bit_ = torch.reshape(bit_, [-1, int(bit_.shape[1] / B), B])
    # num = torch.zeros(bit_[:, :, 1].shape).cuda()
    num = torch.zeros(bit_[:, :, 1].shape)
    for i in range(B):
        num = num + bit_[:, :, i] * 2 ** (B - 1 - i)
    return num


# =======================================================================================================================
# =======================================================================================================================
# Quantization and Dequantization Layers Defining
class Quantization(torch.autograd.Function):
    @staticmethod
    def forward(ctx, x, B):
        ctx.constant = B
        step = 2 ** B
        out = torch.round(x * step - 0.5)
        out = Num2Bit(out, B)
        return out

    @staticmethod
    def backward(ctx, grad_output):
        # return as many input gradients as there were arguments.
        # Gradients of constant arguments to forward must be None.
        # Gradient of a number is the sum of its B bits.
        b, _ = grad_output.shape
        grad_num = torch.sum(grad_output.reshape(b, -1, ctx.constant), dim=2) / ctx.constant
        return grad_num, None


class Dequantization(torch.autograd.Function):
    @staticmethod
    def forward(ctx, x, B):
        ctx.constant = B
        step = 2 ** B
        out = Bit2Num(x, B)
        out = (out + 0.5) / step
        return out

    @staticmethod
    def backward(ctx, grad_output):
        # return as many input gradients as there were arguments.
        # Gradients of non-Tensor arguments to forward must be None.
        # repeat the gradient of a Num for B time.
        b, c = grad_output.shape
        grad_output = grad_output.unsqueeze(2) / ctx.constant
        grad_bit = grad_output.expand(b, c, ctx.constant)
        return torch.reshape(grad_bit, (-1, c * ctx.constant)), None


class QuantizationLayer(nn.Module):
    def __init__(self, B):
        super().__init__()
        self.B = B

    def forward(self, x):
        out = Quantization.apply(x, self.B)
        return out


class DequantizationLayer(nn.Module):
    def __init__(self, B):
        super(DequantizationLayer, self).__init__()
        self.B = B

    def forward(self, x):
        out = Dequantization.apply(x, self.B)
        return out


# =======================================================================================================================
# =======================================================================================================================
# Data Loader Class Defining
class DatasetFolder(Dataset):
    def __init__(self, matInput, matLabel):
        self.input, self.label = matInput, matLabel

    def __getitem__(self, index):
        return self.input[index], self.label[index]

    def __len__(self):
        return self.input.shape[0]


class DatasetFolder_eval(Dataset):
    def __init__(self, data):
        self.data = data

    def __getitem__(self, index):
        return self.data[index]

    def __len__(self):
        return self.data.shape[0]


# =======================================================================================================================
# =======================================================================================================================
# Model Defining
# Encoder and Decoder Class Defining
def conv3x3(in_channels, out_channels, stride=1):
    return nn.Conv2d(in_channels, out_channels, kernel_size=3, stride=stride, padding=1, bias=True)


class ConvBN_linear(nn.Sequential):
    def __init__(self, in_planes, out_planes, kernel_size, stride=1, groups=1):
        if not isinstance(kernel_size, int):
            padding = [(i - 1) // 2 for i in kernel_size]
        else:
            padding = (kernel_size - 1) // 2
        super(ConvBN_linear, self).__init__(OrderedDict([
            ('conv', nn.Conv2d(in_planes, out_planes, kernel_size, stride,
                               padding=padding, groups=groups, bias=False)),
            ('bn', nn.BatchNorm2d(out_planes))
        ]))


class Encoder(nn.Module):
    num_quan_bits = 2

    def __init__(self, feedback_bits):
        super(Encoder, self).__init__()
        self.fc1 = nn.Linear(832, 256)
        self.fc2 = nn.Linear(256, int(feedback_bits / self.num_quan_bits))
        self.sig = nn.Sigmoid()
        self.quantize = QuantizationLayer(self.num_quan_bits)

    def forward(self, x):
        x = x.view(x.shape[0], -1)
        x = x.float()
        out = self.fc1(x)
        out = self.fc2(out)
        out = self.sig(out)
        out = self.quantize(out)
        return out


class Decoder(nn.Module):
    num_quan_bits = 2

    def __init__(self, feedback_bits):
        super(Decoder, self).__init__()
        self.feedback_bits = feedback_bits
        self.dequantize = DequantizationLayer(self.num_quan_bits)
        self.fc = nn.Linear(int(feedback_bits / self.num_quan_bits), 832)
        self.conv1 = conv3x3(2, 32)
        self.conv2 = conv3x3(32, 2)
        self.sig = nn.Sigmoid()
        self.relu = nn.ReLU()

    def forward(self, x):
        out = self.dequantize(x)
        out = self.sig(self.fc(out))
        out = out.reshape(-1, 2, 32, 13)
        out = self.conv1(out)
        out = self.relu(out)
        out = self.conv2(out)
        return out


class AutoEncoder(nn.Module):
    def __init__(self, feedback_bits):
        super(AutoEncoder, self).__init__()
        self.encoder = Encoder(feedback_bits)
        self.decoder = Decoder(feedback_bits)

    def forward(self, x):
        feature = self.encoder(x)
        out = self.decoder(feature)
        return out


# =======================================================================================================================
# =======================================================================================================================
# Function Defining
def encFunction(channel_data, encModel_path):
    """
        Description:
            基于信道矩阵的CSI压缩模块
        Input:
            channel_data: np.array, 信道矩阵数据,  维度[样本数, 子带数*天线数]
            encModel_path: encoder模型读取路径
        Output:
            output_all:  np.array, 编码后的比特流数据，维度[样本数, 反馈比特数]
    """
    num_feedback_bits = 128
    model = AutoEncoder(num_feedback_bits)
    model.encoder.load_state_dict(torch.load(encModel_path, map_location = torch.device('cpu'))['state_dict'])
    eval_dataset = DatasetFolder_eval(channel_data)
    eval_loader = torch.utils.data.DataLoader(eval_dataset, batch_size=32, shuffle=False, num_workers=0,
                                              pin_memory=True)
    model.eval()
    with torch.no_grad():
        for idx, data in enumerate(eval_loader):
            # data = data.cuda()
            modeloutput = model.encoder(data)
            modeloutput = modeloutput.cpu().numpy()
            if idx == 0:
                output_all = modeloutput
            else:
                output_all = np.concatenate((output_all, modeloutput), axis=0)
    return output_all


def decFunction(bits, decModel_path):
    """
        Description:
            基于比特流数据的CSI重建模型
        Input:
            bits: np.array, 反馈的比特流，维度[样本数，反馈比特数]
            decModel_path: decoder模型读取路径
        Output:
            output_all:  np.array, 重构的CSI特征向量矩阵, 维度 [样本数, 2, 子带数, 天线数]
    """
    num_feedback_bits = 128
    model = AutoEncoder(num_feedback_bits)
    model.decoder.load_state_dict(torch.load(decModel_path, map_location = torch.device('cpu'))['state_dict'])
    test_dataset = DatasetFolder_eval(bits)
    test_loader = torch.utils.data.DataLoader(test_dataset, batch_size=32, shuffle=False, num_workers=0,
                                              pin_memory=True)
    model.eval()
    with torch.no_grad():
        for idx, data in enumerate(test_loader):
            # data = data.cuda()
            modeloutput = model.decoder(data)
            modeloutput = modeloutput.cpu().numpy()
            if idx == 0:
                output_all = modeloutput
            else:
                output_all = np.concatenate((output_all, modeloutput), axis=0)
    return output_all


class AEmodelEvaluate:
    def __init__(self, inputdata, encModel_path, decModel_path):
        self.inputdata = inputdata
        self.encModel_path = encModel_path
        self.decModel_path = decModel_path

    def evaluate(self):
        bits = encFunction(self.inputdata, self.encModel_path)
        reconstruct_data = decFunction(bits, self.decModel_path)
        # cal_result(self.inputdata, reconstruct_data)
        # cal_result(self.inputdata, reconstruct_data)
        return reconstruct_data
        # return self.inputdata


class DatasetLoader:
    def __init__(self, inputdata):
        self.inputdata = inputdata
        # print(os.path.abspath('.'))
        # self.los_data_path = self.data_path['los_data_path']
        # self.nlos_data_path = self.data_path['nlos_data_path']
        # self.data_path = self.data_path['beiyou_sim_data_path']
        self.all_data = None
        # self.train_data = None
        # self.test_data = None

    def datareshape(self):
        """
        Data Loading
        load los & nlos
        :return:
        """
        # 这里北邮提供的仿真数据是二维 （50000,416）其中subband_num=13,Antenna_num=32,按照子带嵌套天线维度的顺序排列
        self.all_data = self.split_data(self.inputdata)
        # print('data loading is finished ...')
        # 混合所有不同类型的样本数据
        # self.mixture_all_data()
        # ======================================================================================================================
        # =======================================================================================================================
        # 将数据的实部虚部分开，增加一个维度
        # data 的维度是（样本数，子带数，发送天线数）
        # 要把实部和虚部分开增加一个维度，转化后的维度应为（样本数，子带数，发送天线数，实部虚部）
        self.all_data = np.expand_dims(self.all_data, axis=1)
        self.all_data = np.concatenate((self.all_data.real, self.all_data.imag), axis=1)
        # print('expanded_all_data shape:{all_data_shape}'.format(all_data_shape=self.all_data.shape))
        '''
        # select part of samples for test
        sample_num = 10000
        pilot, w, ht = pilot[:sample_num,...], w[:sample_num,...], ht[:sample_num,...]
        print(pilot.shape, w.shape, ht.shape)
        '''
        # 拆分训练集和测试集
        # self.train_test_split(train_ratio=0.8)
        return self.all_data

    def mixture_all_data(self):
        """
        混合所有不同类型的数据
        :return:
        """
        self.all_data = np.concatenate((self.los_data, self.nlos_data), axis=0)
        # 打乱样本顺序
        np.random.shuffle(self.all_data)
        # debug用，正式训练关闭
        # self.all_data = self.all_data[:10000, :, :]
        print('mixtured all data shape:{all_data_shape}'.format(all_data_shape=self.all_data.shape))

    def train_test_split(self, train_ratio):
        sample_num = int(self.all_data.shape[0] * train_ratio)
        self.train_data = self.all_data[:sample_num, :, :, :]
        self.test_data = self.all_data[sample_num:, :, :, :]

    def split_data(self, sim_data):
        """
        数据是二维 （50000,416）其中subband_num=13,Antenna_num=32,按照子带嵌套天线维度的顺序排列
        将第二维按照子带和天线分开，变成三维
        :return:
        """
        subband_num = 13
        antenna_num = 32
        sim_data = np.expand_dims(sim_data, axis=2)
        tmp = sim_data[:, 0:antenna_num, :]
        for i in range(1, subband_num):
            tmp = np.concatenate((tmp, sim_data[:, i * antenna_num:(i + 1) * antenna_num, :]), axis=2)
        return tmp


# =======================================================================================================================
# =======================================================================================================================
# evaluation score
def cal_result(raw_data, reconstruct_data):
    """
    计算反馈重建精度
    :param raw_data:
    :param reconstruct_data:
    :return:
    """
    total_loss_NMSE = NMSE(reconstruct_data, raw_data)
    total_loss_NMSE = np.mean(total_loss_NMSE)

    reconstruct_data = torch.from_numpy(reconstruct_data)
    raw_data = torch.from_numpy(raw_data)
    total_loss_NMSECUDA = NMSE_cuda(reconstruct_data, raw_data)  # .item()

    total_loss_NMSECUDA = torch.mean(total_loss_NMSECUDA)
    t1 = time.time()

    total_score_cos = cal_score(raw_data, reconstruct_data)
    t2 = time.time()

    # print('NMSE %.4f NMSE_cuda %.4f cossim %.3f cal_score_time %.4f' % (
    # total_loss_NMSE, total_loss_NMSECUDA, total_score_cos, t2 - t1))
    result = tuple([total_loss_NMSE, total_loss_NMSECUDA, total_score_cos])
    # result = complex(total_loss_NMSECUDA, total_score_cos)
    return result


def NMSE(x, x_hat):
    x_real = np.reshape(x[:, 0, :, :], (len(x), -1))
    x_imag = np.reshape(x[:, 1, :, :], (len(x), -1))

    x_hat_real = np.reshape(x_hat[:, 0, :, :], (len(x_hat), -1))
    x_hat_imag = np.reshape(x_hat[:, 1, :, :], (len(x_hat), -1))
    # x_c = x_real - 0.5 + 1j * (x_imag - 012.5)
    # x_hat_c = x_hat_real - 0.5 + 1j * (x_hat_imag - 0.5)
    # power = np.sum(abs(x_c) ** 2, axis=1)
    # mse = np.sum(abs(x_c - x_hat_c) ** 2, axis=1)
    # nmse = np.mean(mse / power)
    # 修改 by lsj
    power = np.sum(x_real ** 2 + x_imag ** 2, axis=1)
    mse = np.sum((x_real - x_hat_real) ** 2 + (x_imag - x_hat_imag) ** 2, axis=1)
    # print(x)
    # print('**************')
    # print(x_hat)
    nmse = mse / power
    return nmse


def NMSE_cuda(x, x_hat):
    x_real = x[:, 0, :, :].view(len(x), -1) - 0.5
    x_imag = x[:, 1, :, :].view(len(x), -1) - 0.5
    x_hat_real = x_hat[:, 0, :, :].contiguous().view(len(x_hat), -1) - 0.5
    x_hat_imag = x_hat[:, 1, :, :].contiguous().view(len(x_hat), -1) - 0.5
    power = torch.sum(x_real ** 2 + x_imag ** 2, dim=1)
    # power = torch.sum(x_real ** 2 + x_imag ** 2, axis=1)
    # mse = torch.sum((x_real - x_hat_real) ** 2 + (x_imag - x_hat_imag) ** 2, axis=1)
    mse = torch.sum((x_real - x_hat_real) ** 2 + (x_imag - x_hat_imag) ** 2, dim=1)
    nmse = mse / power
    return nmse


def rho(x, x_hat):
    x_real = np.reshape(x[:, 0, :, :], (len(x), -1))
    x_imag = np.reshape(x[:, 1, :, :], (len(x), -1))
    x_hat_real = np.reshape(x_hat[:, 0, :, :], (len(x), -1))
    x_hat_imag = np.reshape(x_hat[:, 1, :, :], (len(x), -1))
    cos = nn.CosineSimilarity(dim=1, eps=0)
    out_real = cos(x_real, x_hat_real)
    out_imag = cos(x_imag, x_hat_imag)
    result_real = out_real.sum() / len(out_real)
    reault_imag = out_imag.sum() / len(out_imag)
    return 0.5 * (result_real + reault_imag)

#
# def cos_sim(mat_a, mat_b):
#     mat_a = np.mat(mat_a)
#     mat_b = np.mat(mat_b)
#     num = (np.linalg.norm(mat_a.H * mat_b)) ** 2
#     num1 = (np.linalg.norm(mat_a)) ** 2
#     num2 = (np.linalg.norm(mat_b)) ** 2
#     cos = num / (num1 * num2)
#     return cos
def complex_vector_multiply(mat_a, mat_b):
    data_num = mat_a.shape[1]
    res = 0
    for i in range(0,data_num):
        com_a = complex(mat_a[:,i])
        com_b = complex(mat_b[:,i])
        res = res + com_a*com_b.conjugate()

    return res
def cos_sim(mat_a, mat_b):
    # vector_a = np.mat(mat_a)
    # vector_b = np.mat(mat_b)
    # num = vector_a * vector_b.H
    # num1 = np.sqrt(vector_a * vector_a.H)
    # num2 = np.sqrt(vector_b * vector_b.H)
    # cos = (num / (num1 * num2)).item()

    num = complex_vector_multiply(mat_a, mat_b)
    num1 = np.sqrt(complex_vector_multiply(mat_a, mat_a))
    num2 = np.sqrt(complex_vector_multiply(mat_b, mat_b))
    cos = num / (num1 * num2)

    return cos

def cal_score(w_true, w_pre):
    NUM_SAMPLES = w_true.shape[0]
    NUM_SUBBAND = w_true.shape[3]
    img_total = 2 * w_true.shape[2]  # 64
    num_sample_subband = NUM_SAMPLES * NUM_SUBBAND
    W_true = np.reshape(w_true, [num_sample_subband, img_total])
    W_pre = np.reshape(w_pre, [num_sample_subband, img_total])
    W_true2 = W_true[0:num_sample_subband, 0:int(img_total):2] + 1j * W_true[0:num_sample_subband, 1:int(img_total):2]
    W_pre2 = W_pre[0:num_sample_subband, 0:int(img_total):2] + 1j * W_pre[0:num_sample_subband, 1:int(img_total):2]
    score_cos = 0

    for i in range(num_sample_subband):
        W_true2_sample = W_true2[i:i + 1, ]
        W_pre2_sample = W_pre2[i:i + 1, ]
        score_tmp = cos_sim(W_true2_sample, W_pre2_sample)
        score_cos = score_cos + abs(score_tmp) * abs(score_tmp)

    score_cos = score_cos / num_sample_subband
    return score_cos


# if __name__ == "__main__":
# def model_inference(inputdata):
#     print('aaa')
#     # inputdata = sc.loadmat('/home/chu/桌面/CSI_Simulation_00002.mat')['W_train']
#     inputdata = np.load('/datadir/python_project/lsj/CSI/data/test_data.npz')['arr_0']
#     dataset_loader = DatasetLoader(inputdata)
#     dataset = dataset_loader.datareshape()
#     #dataset = inputdata
#     """模型推理入口"""
#     model_path = '/datadir/python_project/lsj/CSI/model/Csi_Net_Baseline/9'
#     encoder_model_path = os.path.join(model_path, 'encModel.pth.tar')
#     decoder_model_path = os.path.join(model_path, 'decModel.pth.tar')
#     ae_model_eval = AEmodelEvaluate(dataset, encoder_model_path, decoder_model_path)
#     reconstruct_data = ae_model_eval.evaluate()
#     print(reconstruct_data.shape)
#     # cal_result(self.data_val, reconstruct_data)
#
#     return
def model_inference_v3(input):
    t1 = time.time()
    # inputdata3 = sc.loadmat('./inputfiles/CSI_Simulation_00001.mat')['W_train']
    # inputdata = inputdata3[:1,:]
    #最后一位存储AI指标，不参与模型过程
    inputdata = np.array(input[0:-1], ndmin = 2)
    dataset_loader = DatasetLoader(inputdata)
    dataset = dataset_loader.datareshape()

    """模型推理入口"""
    # 拿test_data测试功能，后期需要换成真实的验证数据
    # data_path = Resource().dataset_loader_conf['data_path']
    # data_path = '/home/chu/桌面/CSI_Simulation_00002.mat'
    model_path = './inputfiles/AI_model/model_v4'
    # model_path = os.path.join(os.getcwd(), 'inputfiles/AI_model/model_v3')
    encoder_model_path = os.path.join(model_path, 'encModel.pth.tar')
    decoder_model_path = os.path.join(model_path, 'decModel.pth.tar')
    ae_model_eval = AEmodelEvaluate(dataset, encoder_model_path, decoder_model_path)
    reconstruct_data = ae_model_eval.evaluate()

    #输出AI指标
    metric = cal_result(dataset, reconstruct_data)

    t2 = time.time()
    #输出格式转换
    reshapedata1 = reconstruct_data[:,0,:,:]+1j*reconstruct_data[:,1,:,:]
    reshapedata2 = reshapedata1[0,:,0]
    for i in range(1,13):
        reshapedata2 = np.append(reshapedata2, reshapedata1[0,:,i])
    reshapedata2 = np.append(reshapedata2, metric)
    reshapedata2 = np.append(reshapedata2, t2-t1)
    num_feedback_bits = 128
    reshapedata2 = np.append(reshapedata2, num_feedback_bits)
    reshapedata2 = np.array(reshapedata2, ndmin = 2)
    reshapedata2 = reshapedata2[0,:]
    output = tuple(reshapedata2)

    return output