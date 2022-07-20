/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/fapi_adaptor/mac/messages/ssb.h"

using namespace srsgnb;
using namespace fapi;
using namespace fapi_adaptor;

/// \brief Converts the given ssb_pss_to_sss_epre value into a beta_pss_profile_type value.
///
/// This value corresponds to the "betaPssProfileNR" parameter defined by FAPI.
/// \remark The MAC/Scheduler uses "pss_to_sss_epre" to prevent misusing 3GPP terminology.
static beta_pss_profile_type convert_ssb_pss_to_sss_epre(ssb_pss_to_sss_epre value)
{
  switch (value) {
    case ssb_pss_to_sss_epre::dB_0:
      return beta_pss_profile_type::dB_0;
    case ssb_pss_to_sss_epre::dB_3:
      return beta_pss_profile_type::dB_3;
    default:
      break;
  }
  return beta_pss_profile_type::dB_0;
}

void srsgnb::fapi_adaptor::convert_ssb_mac_to_fapi(fapi::dl_ssb_pdu& fapi_pdu, const srsgnb::dl_ssb_pdu& mac_pdu)
{
  dl_ssb_pdu_builder builder(fapi_pdu);

  convert_ssb_mac_to_fapi(builder, mac_pdu);
}

void srsgnb::fapi_adaptor::convert_ssb_mac_to_fapi(fapi::dl_ssb_pdu_builder& builder, const srsgnb::dl_ssb_pdu& mac_pdu)
{
  builder.set_basic_parameters(mac_pdu.pci,
                               convert_ssb_pss_to_sss_epre(mac_pdu.pss_to_sss_epre),
                               mac_pdu.ssb_index,
                               mac_pdu.subcarrier_offset.to_uint(),
                               mac_pdu.offset_to_pointA.to_uint());

  builder.set_maintenance_v3_basic_parameters(mac_pdu.ssb_case, mac_pdu.scs, mac_pdu.L_max);

  builder.set_maintenance_v3_tx_power_info({}, {});

  const ssb_mib_data_pdu& mib_pdu = mac_pdu.mib_data;
  builder.set_bch_payload_phy_full((mib_pdu.dmrs_typeA_pos == dmrs_typeA_position::pos2) ? 0 : 1,
                                   mib_pdu.pdcch_config_sib1,
                                   mib_pdu.cell_barred,
                                   mib_pdu.intra_freq_reselection);
}
