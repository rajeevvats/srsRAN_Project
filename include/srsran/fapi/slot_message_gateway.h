/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

namespace srsran {
namespace fapi {

struct dl_tti_request_message;
struct tx_data_request_message;
struct ul_dci_request_message;
struct ul_tti_request_message;

/// This interface represents the gateway for sending FAPI slot based messages to the underlying PHY.
class slot_message_gateway
{
public:
  virtual ~slot_message_gateway() = default;

  /// \brief Sends a downlink TTI request message.
  ///
  /// \param[in]  msg   Message contents.
  virtual void dl_tti_request(const dl_tti_request_message& msg) = 0;

  /// \brief Sends an uplink TTI request message.
  ///
  /// \param[in]  msg   Message contents.
  virtual void ul_tti_request(const ul_tti_request_message& msg) = 0;

  /// \brief Sends an uplink DCI request message.
  ///
  /// \param[in]  msg   Message contents.
  virtual void ul_dci_request(const ul_dci_request_message& msg) = 0;

  /// \brief Sends a TX data request message.
  ///
  /// \param[in]  msg   Message contents.
  virtual void tx_data_request(const tx_data_request_message& msg) = 0;
};

} // namespace fapi
} // namespace srsran