#include "BTSID.h"
#include "../MobileStation/MSID.h"
#include "../MobileStation/HARQRxStateMS.h"
#include "../MobileStation/HARQTxStateMS.h"
#include "../MobileStation/MS.h"
#include "MSRxBufferBS.h"

MSRxBufferBS::MSRxBufferBS(const MSID& _id)
: m_iMSID(_id) {
}