package com.picross.nonocross.views

import android.content.Context
import android.view.LayoutInflater
import android.widget.EditText
import androidx.appcompat.app.AlertDialog
import androidx.core.widget.doOnTextChanged
import com.google.android.material.dialog.MaterialAlertDialogBuilder
import com.picross.nonocross.R

class LevelSaveDialog {
    companion object {
        fun showDialog(context: Context, layoutInflater: LayoutInflater, finished: (String) -> Unit) {
            val textInputLayout = layoutInflater.inflate(R.layout.edit_text_layout, null)
            val textInput = textInputLayout.findViewById<EditText>(R.id.edit_level_name)

            val dialog = MaterialAlertDialogBuilder(context)
                .setTitle(R.string.enter_level_name)
                .setView(textInputLayout)
                .setPositiveButton(
                    android.R.string.ok
                ) { _, _ ->
                    val fileName = textInput.text?.toString() ?: context.getString(R.string.untitled_level)
                    finished(fileName)
                }
                .setNegativeButton(android.R.string.cancel, null)
                .create()

            dialog.show()

            val okButton = dialog.getButton(AlertDialog.BUTTON_POSITIVE)
            okButton.isEnabled = false
            textInput.doOnTextChanged { text, _, _, _ ->
                okButton.isEnabled = (text?.length ?: 0) > 0
            }
        }
    }
}