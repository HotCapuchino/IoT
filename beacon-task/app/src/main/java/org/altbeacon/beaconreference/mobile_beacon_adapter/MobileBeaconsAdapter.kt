package org.altbeacon.beaconreference.mobile_beacon_adapter

import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import androidx.recyclerview.widget.RecyclerView
import org.altbeacon.beacon.Beacon
import org.altbeacon.beaconreference.R
import org.altbeacon.beaconreference.databinding.MobileBeaconItemBinding

class MobileBeaconsAdapter(private var mobileBeacons: MutableMap<String, Pair<String, Double>>): RecyclerView.Adapter<MobileBeaconsAdapter.MyHolder>() {

    class MyHolder(itemView: View) : RecyclerView.ViewHolder(itemView) {
        private val binding = MobileBeaconItemBinding.bind(itemView)

        fun bind(mobileBeaconId: String, closestStationary: Pair<String, Double>) {
            val (stationaryId, distance) = closestStationary
            binding.mobileBeaconId.text = "Id: $mobileBeaconId"
            if (stationaryId == "None") {
                binding.closestStationary.text = "No closest stationary detected"
            } else {
                binding.closestStationary.text = "Id: $stationaryId, dist: $distance"
            }
        }
    }

    override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): MyHolder {
        val view = LayoutInflater.from(parent.context).inflate(R.layout.mobile_beacon_item, parent, false)
        return MyHolder(view)
    }

    override fun onBindViewHolder(holder: MyHolder, position: Int) {
        val key = mobileBeacons.keys.toTypedArray()[position]
        val pair = mobileBeacons[key] ?: ("None" to Double.MAX_VALUE)
        holder.bind(key, pair)
    }

    override fun getItemCount(): Int {
        return mobileBeacons.size
    }
}