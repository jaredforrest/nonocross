package com.picross.nonocross

import android.os.Bundle
import android.os.Handler
import android.os.Looper
import android.view.View
import androidx.appcompat.app.AppCompatActivity
import androidx.appcompat.app.AppCompatDelegate
import androidx.fragment.app.commit
import androidx.preference.Preference
import androidx.preference.PreferenceFragmentCompat
import com.mikepenz.aboutlibraries.LibsBuilder
import com.mikepenz.aboutlibraries.LibsConfiguration
import com.mikepenz.aboutlibraries.entity.Library
import com.mikepenz.aboutlibraries.util.SpecialButton
import com.picross.nonocross.util.Preferences
import com.picross.nonocross.util.openInBrowser

class SettingsActivity : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.settings_activity)
        if (savedInstanceState == null) {
            supportFragmentManager
                .beginTransaction()
                .replace(R.id.settings, SettingsFragment())
                .commit()
        }
        supportActionBar?.setDisplayHomeAsUpEnabled(true)
    }

    class SettingsFragment : PreferenceFragmentCompat() {
        override fun onCreatePreferences(savedInstanceState: Bundle?, rootKey: String?) {
            setPreferencesFromResource(R.xml.root_preferences, rootKey)

            val themePref = preferenceManager.findPreference<Preference>("darkMode2")!!
            themePref.setOnPreferenceChangeListener { preference, _ ->
                Handler(Looper.getMainLooper()).post {
                    AppCompatDelegate.setDefaultNightMode(Preferences(preference.context).theme)
                }
                true
            }

            val aboutPref = preferenceManager.findPreference<Preference>("about")!!
            aboutPref.setOnPreferenceClickListener {
                val fragment = LibsBuilder()
                    .withAboutIconShown(true)
                    .withAboutVersionShownName(true)
                    .withSortEnabled(true)
                    .withListener(AboutButtonsListener())
                    .supportFragment()

                parentFragmentManager.commit {
                    addToBackStack("about")
                    replace(R.id.settings, fragment, "about")
                }
                true
            }
        }
    }
}

class AboutButtonsListener : LibsConfiguration.LibsListener {
    override fun onExtraClicked(v: View, specialButton: SpecialButton): Boolean {
        val link = when (specialButton) {
            SpecialButton.SPECIAL1 -> "https://github.com/jaredforrest/nonocross/"
            SpecialButton.SPECIAL2 -> "https://f-droid.org/packages/com.picross.nonocross/"
            SpecialButton.SPECIAL3 -> null
        }
        link?.openInBrowser(v.context)
        return true
    }

    override fun onIconClicked(v: View) {
        // no-op
    }

    override fun onIconLongClicked(v: View): Boolean {
        return false
    }

    override fun onLibraryAuthorClicked(v: View, library: Library): Boolean {
        return false
    }

    override fun onLibraryAuthorLongClicked(v: View, library: Library): Boolean {
        return false
    }

    override fun onLibraryBottomClicked(v: View, library: Library): Boolean {
        return false
    }

    override fun onLibraryBottomLongClicked(v: View, library: Library): Boolean {
        return false
    }

    override fun onLibraryContentClicked(v: View, library: Library): Boolean {
        return false
    }

    override fun onLibraryContentLongClicked(v: View, library: Library): Boolean {
        return false
    }
}