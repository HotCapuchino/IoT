package org.altbeacon.beaconreference

import org.altbeacon.beacon.Beacon

data class CustomBeacon(val beacon: Beacon, var isStationary: Boolean, var isMobile: Boolean)

enum class ControlButtonsParams {
    TARGET,
    STATIONARY,
    MOBILE
}