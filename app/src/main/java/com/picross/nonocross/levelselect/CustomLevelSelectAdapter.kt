/**This file is part of Nonocross.

Nonocross is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Nonocross is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Nonocross.  If not, see <https://www.gnu.org/licenses/>.*/
package com.picross.nonocross.levelselect

import android.content.Context
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.Button
import android.widget.LinearLayout
import android.widget.TextView
import androidx.core.view.get
import androidx.recyclerview.widget.RecyclerView
import com.picross.nonocross.LevelDetails
import com.picross.nonocross.LevelType
import com.picross.nonocross.R
import com.picross.nonocross.util.usergrid.GridData
import com.picross.nonocross.util.usergrid.UserGrid
import kotlinx.collections.immutable.PersistentList
import kotlinx.coroutines.runBlocking


class CustomLevelSelectAdapter(
    private val startGame: StartGame,
    private var context: Context,
) :
    RecyclerView.Adapter<CustomLevelSelectAdapter.MyViewHolder>() {

    // Provide a reference to the views for each data item
    // Complex data items may need more than one view per item, and
    // you provide access to all the views for a data item in a view holder.
    // Each data item is just a string in this case that is shown in a TextView.
    class MyViewHolder(levelView: View) : RecyclerView.ViewHolder(levelView)


    // Create new views (invoked by the layout manager)
    override fun onCreateViewHolder(
        parent: ViewGroup,
        viewType: Int
    ): MyViewHolder {
        // create a new view
        val levelView = LayoutInflater.from(parent.context)
            .inflate(R.layout.level_view, parent, false) as View
        // set the view's size, margins, padding and layout parameters
        //...
        levelView.setPadding(20, 60, 20, 40)

        // Remove "remove level" button for default levels
        if (startGame.levels is PersistentList) {
            (levelView as LinearLayout)[3].visibility = View.GONE
        }
        return MyViewHolder(levelView)
    }

    interface StartGame {
        fun startGame()
        fun removeLevel(level: String, position: Int)
        fun openSave(levelName: String): ByteArray

        fun levelType(levelName: String): LevelType

        val levels: List<Pair<String, GridData>>

    }

    // Replace the contents of a view
    override fun onBindViewHolder(holder: MyViewHolder, position: Int) =

        runBlocking {
            val levelName = startGame.levels[position].first //levels[position]
            //val level = openGridFile(context, levelName, isCustom)
            val level = startGame.levels[position].second //startGame.openGrid(levelName)
            holder.itemView.findViewById<TextView>(R.id.level_name).text = levelName
            holder.itemView.findViewById<TextView>(R.id.gridData).text = context.getString(
                R.string.height_x_width,
                level.height,
                level.width
            )
            holder.itemView.findViewById<Button>(R.id.level_select).setOnClickListener {
                LevelDetails.levelType = startGame.levelType(levelName)
//                    if (isCustom) LevelType.Custom(levelName) else
                LevelDetails.gridData = level
                LevelDetails.userGrid =
                    UserGrid(LevelDetails.gridData, startGame.openSave(levelName),
                        LevelDetails.levelType !is LevelType.Default
                    )
                startGame.startGame()
            }
            if (startGame.levels is MutableList) {
                holder.itemView.findViewById<Button>(R.id.remove_level).setOnClickListener {
                    val pos = holder.bindingAdapterPosition
                    (startGame.levels as MutableList<Pair<String, GridData>>).removeAt(pos)
                    startGame.removeLevel(levelName, pos)
                }
            }
        }

    // Return the size of levels List
    override fun getItemCount() = startGame.levels.size
}

