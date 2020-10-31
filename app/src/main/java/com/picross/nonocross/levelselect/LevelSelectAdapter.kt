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
import android.widget.TextView
import androidx.recyclerview.widget.RecyclerView
import com.picross.nonocross.R
import com.picross.nonocross.util.openGridFile


class LevelSelectAdapter(
    private val levels: List<String>,
    private val startGame: StartGame,
    private var context: Context
) :
    RecyclerView.Adapter<LevelSelectAdapter.MyViewHolder>() {

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
        return MyViewHolder(levelView)
    }

    interface StartGame{
        fun startGame(levelName: String)
    }

    // Replace the contents of a view
    override fun onBindViewHolder(holder: MyViewHolder, position: Int) {
        val level = openGridFile(context, levels[position])
        holder.itemView.findViewById<TextView>(R.id.level_name).text = levels[position]
        holder.itemView.findViewById<TextView>(R.id.gridData).text = context.getString(
            R.string.width_x_height,
            level.size,
            level[0].size
        )
        holder.itemView.findViewById<Button>(R.id.level_select).setOnClickListener { startGame.startGame(
            levels[position]
        ) }
    }

    // Return the size of levels List
    override fun getItemCount() = levels.size
}

