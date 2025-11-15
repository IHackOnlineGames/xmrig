#ifndef XMRIG_DONATE_H
#define XMRIG_DONATE_H

//
// XMRig Donation System
//
// kDefaultDonateLevel — the default percentage of hashing time donated
// kMinimumDonateLevel — the minimum allowed donation percentage
//
// These values can be set to 0, but users are encouraged to support development.
//

namespace xmrig::donate {

// Default developer donation level (percent).
inline constexpr int kDefaultDonateLevel = 1;

// Minimum allowed donation level (percent).
inline constexpr int kMinimumDonateLevel = 1;

} // namespace xmrig::donate

#endif // XMRIG_DONATE_H
