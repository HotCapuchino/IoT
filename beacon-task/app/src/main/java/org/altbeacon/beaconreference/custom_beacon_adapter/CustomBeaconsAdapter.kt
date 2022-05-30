package org.altbeacon.beaconreference.custom_beacon_adapter

import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import androidx.recyclerview.widget.RecyclerView
import org.altbeacon.beaconreference.ControlButtonsParams
import org.altbeacon.beaconreference.CustomBeacon
import org.altbeacon.beaconreference.R
import org.altbeacon.beaconreference.databinding.CustomBeaconItemBinding
import kotlin.math.roundToInt

class CustomBeaconsAdapter(private var allBeacon: ArrayList<CustomBeacon>, val btnCallback: (id: String, param: ControlButtonsParams) -> Unit): RecyclerView.Adapter<CustomBeaconsAdapter.MyHolder>() {
    var idx : ArrayList<String> = ArrayList()

    class MyHolder(itemView: View) : RecyclerView.ViewHolder(itemView) {
        private val binding = CustomBeaconItemBinding.bind(itemView)
        private var beaconId: String = ""

        fun bind(customBeacon: CustomBeacon, id: String) = with(binding) {
            binding.beaconText.text = "id: ${customBeacon.beacon.id3}\ndist: ${(customBeacon.beacon.distance * 100.0).roundToInt() / 100.0}m"
            beaconId = id

            binding.isStationary.text = if (customBeacon.isStationary) "Is Stationary: Yes" else "Is Stationary: No"
            binding.makeStationary.text = if (customBeacon.isStationary) "Remove Stationary" else "Make Stationary"
            binding.makeMobile.isEnabled = !customBeacon.isStationary

            binding.isMobile.text = if (customBeacon.isMobile) "Is Mobile: Yes" else "Is Mobile: No"
            binding.makeMobile.text = if (customBeacon.isMobile) "Remove Mobile" else "Make Mobile"
            binding.makeStationary.isEnabled = !customBeacon.isMobile

            if (btnCallback != null) {
                binding.makeTarget.setOnClickListener { btnCallback?.let { it1 -> it1(beaconId,
                    ControlButtonsParams.TARGET
                ) } }
                binding.makeStationary.setOnClickListener {
                    val prevState = customBeacon.isStationary
                    btnCallback?.let { it1 -> it1(beaconId, ControlButtonsParams.STATIONARY) }

                    binding.makeStationary.text = if (!prevState) "Remove Stationary" else "Make Stationary"
                    binding.makeMobile.isEnabled = prevState
                }
                binding.makeMobile.setOnClickListener {
                    val prevState = customBeacon.isMobile
                    btnCallback?.let { it1 -> it1(beaconId, ControlButtonsParams.MOBILE) }

                    binding.makeMobile.text = if (!prevState) "Remove Mobile" else "Make Mobile"
                    binding.makeMobile.isEnabled = prevState
                }
            }
        }

        companion object {
            var btnCallback: ((id: String, param: ControlButtonsParams) -> Unit)? = null
        }
    }

    override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): MyHolder {
        val view = LayoutInflater.from(parent.context).inflate(R.layout.custom_beacon_item, parent, false)
        val holder = MyHolder(view)
        MyHolder.btnCallback = btnCallback
        return holder
    }

    override fun onBindViewHolder(holder: MyHolder, position: Int) {
        holder.bind(allBeacon[position], idx[position])
    }

    override fun getItemCount(): Int {
        return allBeacon.size
    }

    fun addBeacon(customBeacon: CustomBeacon) {
        val id = customBeacon.beacon.id3.toString()

        if (!idx.contains(id)) {
            idx.add(id)
            allBeacon.add(idx.indexOf(id), customBeacon)
        } else {
            val beaconToRemove = allBeacon.find { it.beacon == customBeacon.beacon }
            if (beaconToRemove != null) {
                allBeacon.remove(beaconToRemove)

                customBeacon.isStationary = beaconToRemove.isStationary
                customBeacon.isMobile = beaconToRemove.isMobile
                allBeacon.add(idx.indexOf(id), customBeacon)
            }
        }

        notifyDataSetChanged()
    }
}