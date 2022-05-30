package org.altbeacon.beaconreference

import android.Manifest
import android.app.AlertDialog
import android.content.pm.PackageManager
import android.graphics.Color
import android.os.Build
import android.os.Bundle
import android.util.Log
import android.view.View
import android.widget.*
import androidx.appcompat.app.AppCompatActivity
import androidx.lifecycle.Observer
import androidx.recyclerview.widget.LinearLayoutManager
import kotlinx.android.synthetic.main.activity_main.*
import org.altbeacon.beacon.Beacon
import org.altbeacon.beacon.BeaconManager
import org.altbeacon.beacon.MonitorNotifier
import org.altbeacon.beaconreference.custom_beacon_adapter.CustomBeaconsAdapter
import org.altbeacon.beaconreference.databinding.ActivityMainBinding
import org.altbeacon.beaconreference.mobile_beacon_adapter.MobileBeaconsAdapter
import kotlin.math.abs
import kotlin.math.round

class MainActivity : AppCompatActivity() {
    lateinit var beaconCountTextView: TextView
    private lateinit var resultTextView: TextView
    private lateinit var monitoringButton: Button
    private lateinit var rangingButton: Button
    private lateinit var beaconReferenceApplication: BeaconReferenceApplication
    private var alertDialog: AlertDialog? = null
    var neverAskAgainPermissions = ArrayList<String>()

    lateinit var binding: ActivityMainBinding
    private var allBeacon: ArrayList<CustomBeacon> = arrayListOf()
    private val adapter = CustomBeaconsAdapter(allBeacon, ::makeTargetClick)
    private var mobileBeaconsMap = mutableMapOf<String, Pair<String, Double>>()
    private val mobileBeaconsAdapter = MobileBeaconsAdapter(mobileBeaconsMap)
    private var beaconDistances: MutableMap<String, ArrayList<Double>> = mutableMapOf()

    var targetId: String = ""

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)
        beaconReferenceApplication = application as BeaconReferenceApplication

        // Set up a Live Data observer for beacon data
        val regionViewModel = BeaconManager.getInstanceForApplication(this).getRegionViewModel(beaconReferenceApplication.region)
        // observer will be called each time the monitored regionState changes (inside vs. outside region)
        regionViewModel.regionState.observe(this, monitoringObserver)
        // observer will be called each time a new list of beacons is ranged (typically ~1 second in the foreground)
        regionViewModel.rangedBeacons.observe(this, rangingObserver)

        initializeElements()

        beaconCountTextView.text = "No beacons detected"
        binding.beaconList.layoutManager = LinearLayoutManager(this)
        binding.beaconList.adapter = adapter
        binding.mobileBeacons.layoutManager = LinearLayoutManager(this)
        binding.mobileBeacons.adapter = mobileBeaconsAdapter
    }

    fun initializeElements() {
        rangingButton = findViewById(R.id.rangingButton)
        monitoringButton = findViewById(R.id.monitoringButton)
        beaconCountTextView = findViewById(R.id.beaconCount)
        resultTextView = findViewById(R.id.textResult)
    }

    override fun onPause() {
        Log.d(TAG, "onPause")
        super.onPause()
    }
    override fun onResume() {
        Log.d(TAG, "onResume")
        super.onResume()
        checkPermissions()
    }

    private val monitoringObserver = Observer<Int> { state ->
        var dialogTitle = "Beacons detected"
        var dialogMessage = "didEnterRegionEvent has fired"

        if (state == MonitorNotifier.OUTSIDE) {
            dialogTitle = "No beacons detected"
            dialogMessage = "didExitRegionEvent has fired"
            beaconCountTextView.text = "Outside of the beacon region -- no beacons detected"
        }
        else {
            beaconCountTextView.text = "Inside the beacon region."
        }

        val builder = AlertDialog.Builder(this).also {
            it.setTitle(dialogTitle)
            it.setMessage(dialogMessage)
            it.setPositiveButton(android.R.string.ok, null)
        }

        alertDialog?.dismiss()
        alertDialog = builder.create()
        alertDialog?.show()
    }

    private val rangingObserver = Observer<Collection<Beacon>> { beacons ->
        if (BeaconManager.getInstanceForApplication(this).rangedRegions.isNotEmpty()) {

            beacons.map {
                if (it.id3.toString().isNotEmpty() || it.id3.toString().isNotBlank()) {
                    val customBeacon = CustomBeacon(it, false, false)
                    adapter.addBeacon(customBeacon)
                }
            }

            val stationaryBeacons = allBeacon.filter { it.isStationary }.map { it.beacon }
            val mobileBeacons = allBeacon.filter { it.isMobile }.map { it.beacon }

            calculateDistances(stationaryBeacons, mobileBeacons)

            val currentBeacon = beacons.filter {(it.id3).toString() == targetId}
            if (currentBeacon.isNotEmpty()) {
                if ((currentBeacon[0].distance).toFloat() < 0.2) {
                    resultTextView.text = "You've reached target beacon"
                    resultTextView.setTextColor(Color.GREEN)
                    target_beacon_container.setBackgroundColor(Color.parseColor("#666FF44A"))
                } else {
                    resultTextView.text = "Target beacon hasn't been reached"
                    resultTextView.setTextColor(Color.RED)
                    target_beacon_container.setBackgroundColor(Color.parseColor("#66EF2121"))
                }
            }

            beaconCountTextView.text = "Ranging enabled: ${beacons.count()} beacon(s) detected"
        }
    }

    private fun calculateDistances(stationaryBeacons: List<Beacon>, mobileBeacons: List<Beacon>) {
        stationaryBeacons.forEach(mapFuncDistanceAppending)
        mobileBeacons.forEach(mapFuncDistanceAppending)

        val stationaryDistances = stationaryBeacons.map(mapFuncDistanceSum)
        val mobileDistances = mobileBeacons.map(mapFuncDistanceSum)

        Log.d("calculateDistances stationary length", stationaryDistances.size.toString())
        Log.d("calculateDistances mobile length", mobileDistances.size.toString())

        val minDist: MutableMap<String, Pair<String, Double>> = mobileBeacons.associate { it.id3.toString() to ("None" to Double.MAX_VALUE) }.toMutableMap()
        mobileDistances.forEach { mobPair ->
            val (mobId, mobDist) = mobPair
            stationaryDistances.forEach { stPair ->
                val (stId, stDist) = stPair

                if (stDist > 0.0) {
                    val currentMinDist: Double = minDist[mobId]?.second!!
                    val calcMinDist = Math.round( abs(stDist - mobDist) * 100.0) / 100.0
                    Log.d("calcMinDist", "dist from $mobId to $stId is $calcMinDist")

                    if (calcMinDist < currentMinDist) {
                        minDist[mobId] = stId to calcMinDist
                    }
                }
            }
        }

        Log.d("minDist", minDist.toString())
//
//        mobileBeaconsMap = minDist
//        mobileBeaconsAdapter.notifyDataSetChanged()
    }

    private val mapFuncDistanceAppending = { it: Beacon ->
        val id = it.id3.toString()
        if (beaconDistances.contains(id)) {
            beaconDistances[id]?.add(it.distance)
            Log.d("mapFuncDistanceAppending", "beacon distances already contains this $id id")
        } else {
            val newList = ArrayList<Double>()
            newList.add(it.distance)
            beaconDistances[id] = newList
            Log.d("mapFuncDistanceAppending", "beacon distances does not contain this $id id")
        }
        Unit
    }

    private val mapFuncDistanceSum = { it: Beacon ->
        val distancesArray = beaconDistances[it.id3.toString()]
        var sum = -1.0
        if (distancesArray != null) {
            if (distancesArray.size == 2) {
                Log.d("mapFuncDistanceSum", "distance has fulfilled")
                sum = 0.0
                sum = distancesArray[0] - distancesArray[1]

                distancesArray.clear()
//                beaconDistances[it.id3.toString()] = distancesArray
            }
        }
        it.id3.toString() to sum
    }

    fun rangingButtonClick(view: View) {
        val beaconManager = BeaconManager.getInstanceForApplication(this)
        if (beaconManager.rangedRegions.isEmpty()) {
            beaconManager.startRangingBeacons(beaconReferenceApplication.region)
            rangingButton.text = "Stop Ranging"
            beaconCountTextView.text = "Ranging enabled -- awaiting first callback"
        }
        else {
            beaconManager.stopRangingBeacons(beaconReferenceApplication.region)
            rangingButton.text = "Start Ranging"
            beaconCountTextView.text = "Ranging disabled -- no beacons detected"
        }
    }

    fun monitoringButtonClick(view: View) {
        var dialogTitle = ""
        var dialogMessage = ""
        val beaconManager = BeaconManager.getInstanceForApplication(this)
        if (beaconManager.monitoredRegions.isEmpty()) {
            beaconManager.startMonitoring(beaconReferenceApplication.region)
            dialogTitle = "Beacon monitoring started."
            dialogMessage = "You will see a dialog if a beacon is detected, and another if beacons then stop being detected."
            monitoringButton.text = "Stop Monitoring"

        }
        else {
            beaconManager.stopMonitoring(beaconReferenceApplication.region)
            dialogTitle = "Beacon monitoring stopped."
            dialogMessage = "You will no longer see dialogs when becaons start/stop being detected."
            monitoringButton.text = "Start Monitoring"
        }
        val builder = AlertDialog.Builder(this).also {
            it.setTitle(dialogTitle)
            it.setMessage(dialogMessage)
            it.setPositiveButton(android.R.string.ok, null)
        }

        alertDialog?.dismiss()
        alertDialog = builder.create()
        alertDialog?.show()

    }

    private fun makeTargetClick(id: String, param: ControlButtonsParams) {
        Log.d("TargetID", id)
        val position = adapter.idx.indexOf(id)
        Log.d("TargetIndex", position.toString())

        if (position >= 0) {
            val targetBeacon = allBeacon[position]
            when (param) {
                ControlButtonsParams.TARGET -> {
                    Log.d("TARGET", position.toString())
                    targetId = id
                    binding.targetBeaconContainer.visibility = View.VISIBLE
                    binding.targetBeacon.text = "id: ${targetBeacon.beacon.id3}"
                }
                ControlButtonsParams.STATIONARY -> {
                    targetBeacon.isStationary = !targetBeacon.isStationary
                    Log.d("STATIONARY", position.toString())
                    adapter.notifyDataSetChanged()
                }
                ControlButtonsParams.MOBILE -> {
                    targetBeacon.isMobile = !targetBeacon.isMobile

                    if (targetBeacon.isMobile) {
                        mobileBeaconsMap[targetBeacon.beacon.id3.toString()] = "None" to Double.MAX_VALUE
                    } else {
                        val key = mobileBeaconsMap.keys.toTypedArray().find { it == targetBeacon.beacon.id3.toString() }
                        mobileBeaconsMap.remove(key)
                    }

                    binding.mobileBeaconsText.visibility = if (mobileBeaconsMap.isEmpty()) View.VISIBLE else View.GONE

                    Log.d("MOBILE", position.toString())
                    adapter.notifyDataSetChanged()
                    mobileBeaconsAdapter.notifyDataSetChanged()
                }
            }
        }
    }

    fun removeTargetClick(view: View) {
        targetId = ""

        binding.targetBeaconContainer.visibility = View.GONE
        resultTextView.text = "Target beacon hasn't been reached"
        resultTextView.setTextColor(Color.RED)
    }

    override fun onRequestPermissionsResult(
        requestCode: Int,
        permissions: Array<out String>,
        grantResults: IntArray
    ) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults)
        for (i in 1 until permissions.size) {
            Log.d(TAG, "onRequestPermissionResult for "+permissions[i]+":" +grantResults[i])
            if (grantResults[i] != PackageManager.PERMISSION_GRANTED) {
                //check if user select "never ask again" when denying any permission
                if (!shouldShowRequestPermissionRationale(permissions[i])) {
                    neverAskAgainPermissions.add(permissions[i])
                }
            }
        }
    }


    fun checkPermissions() {
        // base permissions are for M and higher
        var permissions = arrayOf( Manifest.permission.ACCESS_FINE_LOCATION)
        var permissionRationale ="This app needs fine location permission to detect beacons.  Please grant this now."
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
            permissions = arrayOf( Manifest.permission.ACCESS_FINE_LOCATION, Manifest.permission.BLUETOOTH_SCAN)
            permissionRationale ="This app needs fine location permission, and bluetooth scan permission to detect beacons.  Please grant all of these now."
        }
        else if (android.os.Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
            if ((checkSelfPermission(Manifest.permission.ACCESS_FINE_LOCATION) != PackageManager.PERMISSION_GRANTED)) {
                permissions = arrayOf( Manifest.permission.ACCESS_FINE_LOCATION)
                permissionRationale ="This app needs fine location permission to detect beacons.  Please grant this now."
            }
            else {
                permissions = arrayOf( Manifest.permission.ACCESS_BACKGROUND_LOCATION)
                permissionRationale ="This app needs background location permission to detect beacons in the background.  Please grant this now."
            }
        }
        else if (android.os.Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q) {
            permissions = arrayOf( Manifest.permission.ACCESS_FINE_LOCATION, Manifest.permission.ACCESS_BACKGROUND_LOCATION)
            permissionRationale ="This app needs both fine location permission and background location permission to detect beacons in the background.  Please grant both now."
        }
        var allGranted = true
        for (permission in permissions) {
            if (checkSelfPermission(permission) != PackageManager.PERMISSION_GRANTED) allGranted = false;
        }
        if (!allGranted) {
            if (neverAskAgainPermissions.isEmpty()) {
                val builder =
                    AlertDialog.Builder(this)
                builder.setTitle("This app needs permissions to detect beacons")
                builder.setMessage(permissionRationale)
                builder.setPositiveButton(android.R.string.ok, null)
                builder.setOnDismissListener {
                    requestPermissions(
                        permissions,
                        PERMISSION_REQUEST_FINE_LOCATION
                    )
                }
                builder.show()
            }
            else {
                val builder =
                    AlertDialog.Builder(this)
                builder.setTitle("Functionality limited")
                builder.setMessage("Since location and device permissions have not been granted, this app will not be able to discover beacons.  Please go to Settings -> Applications -> Permissions and grant location and device discovery permissions to this app.")
                builder.setPositiveButton(android.R.string.ok, null)
                builder.setOnDismissListener { }
                builder.show()
            }
        }
        else {
            if (android.os.Build.VERSION.SDK_INT > Build.VERSION_CODES.P) {
                if (checkSelfPermission(Manifest.permission.ACCESS_BACKGROUND_LOCATION)
                    != PackageManager.PERMISSION_GRANTED
                ) {
                    if (shouldShowRequestPermissionRationale(Manifest.permission.ACCESS_BACKGROUND_LOCATION)) {
                        val builder =
                            AlertDialog.Builder(this)
                        builder.setTitle("This app needs background location access")
                        builder.setMessage("Please grant location access so this app can detect beacons in the background.")
                        builder.setPositiveButton(android.R.string.ok, null)
                        builder.setOnDismissListener {
                            requestPermissions(
                                arrayOf(Manifest.permission.ACCESS_BACKGROUND_LOCATION),
                                PERMISSION_REQUEST_BACKGROUND_LOCATION
                            )
                        }
                        builder.show()
                    } else {
                        val builder =
                            AlertDialog.Builder(this)
                        builder.setTitle("Functionality limited")
                        builder.setMessage("Since background location access has not been granted, this app will not be able to discover beacons in the background.  Please go to Settings -> Applications -> Permissions and grant background location access to this app.")
                        builder.setPositiveButton(android.R.string.ok, null)
                        builder.setOnDismissListener { }
                        builder.show()
                    }
                }
            }
            else if (android.os.Build.VERSION.SDK_INT > Build.VERSION_CODES.S &&
                (checkSelfPermission(Manifest.permission.BLUETOOTH_SCAN)
                        != PackageManager.PERMISSION_GRANTED)) {
                if (shouldShowRequestPermissionRationale(Manifest.permission.BLUETOOTH_SCAN)) {
                    val builder =
                        AlertDialog.Builder(this)
                    builder.setTitle("This app needs bluetooth scan permission")
                    builder.setMessage("Please grant scan permission so this app can detect beacons.")
                    builder.setPositiveButton(android.R.string.ok, null)
                    builder.setOnDismissListener {
                        requestPermissions(
                            arrayOf(Manifest.permission.BLUETOOTH_SCAN),
                            PERMISSION_REQUEST_BLUETOOTH_SCAN
                        )
                    }
                    builder.show()
                } else {
                    val builder =
                        AlertDialog.Builder(this)
                    builder.setTitle("Functionality limited")
                    builder.setMessage("Since bluetooth scan permission has not been granted, this app will not be able to discover beacons  Please go to Settings -> Applications -> Permissions and grant bluetooth scan permission to this app.")
                    builder.setPositiveButton(android.R.string.ok, null)
                    builder.setOnDismissListener { }
                    builder.show()
                }
            }
            else {
                if (android.os.Build.VERSION.SDK_INT > Build.VERSION_CODES.P) {
                    if (checkSelfPermission(Manifest.permission.ACCESS_BACKGROUND_LOCATION)
                        != PackageManager.PERMISSION_GRANTED
                    ) {
                        if (shouldShowRequestPermissionRationale(Manifest.permission.ACCESS_BACKGROUND_LOCATION)) {
                            val builder =
                                AlertDialog.Builder(this)
                            builder.setTitle("This app needs background location access")
                            builder.setMessage("Please grant location access so this app can detect beacons in the background.")
                            builder.setPositiveButton(android.R.string.ok, null)
                            builder.setOnDismissListener {
                                requestPermissions(
                                    arrayOf(Manifest.permission.ACCESS_BACKGROUND_LOCATION),
                                    PERMISSION_REQUEST_BACKGROUND_LOCATION
                                )
                            }
                            builder.show()
                        } else {
                            val builder =
                                AlertDialog.Builder(this)
                            builder.setTitle("Functionality limited")
                            builder.setMessage("Since background location access has not been granted, this app will not be able to discover beacons in the background.  Please go to Settings -> Applications -> Permissions and grant background location access to this app.")
                            builder.setPositiveButton(android.R.string.ok, null)
                            builder.setOnDismissListener { }
                            builder.show()
                        }
                    }
                }
            }
        }

    }

    companion object {
        const val TAG = "MainActivity"
        const val PERMISSION_REQUEST_BACKGROUND_LOCATION = 0
        const val PERMISSION_REQUEST_BLUETOOTH_SCAN = 1
        val PERMISSION_REQUEST_BLUETOOTH_CONNECT = 2
        const val PERMISSION_REQUEST_FINE_LOCATION = 3
    }

}