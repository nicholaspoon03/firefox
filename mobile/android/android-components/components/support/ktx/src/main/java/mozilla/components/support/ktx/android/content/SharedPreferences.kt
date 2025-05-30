/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

@file:Suppress("MatchingDeclarationName")

package mozilla.components.support.ktx.android.content

import android.content.SharedPreferences
import androidx.core.content.edit
import kotlin.properties.ReadWriteProperty
import kotlin.reflect.KProperty

/**
 * Represents a class that holds a reference to [SharedPreferences].
 */
interface PreferencesHolder {
    val preferences: SharedPreferences
}

private class BooleanPreference(
    private val key: String,
    private val default: () -> Boolean,
    private val persistDefaultIfNotExists: Boolean = false,
) : ReadWriteProperty<PreferencesHolder, Boolean> {

    override fun getValue(thisRef: PreferencesHolder, property: KProperty<*>): Boolean =
        if (thisRef.preferences.contains(key)) {
            thisRef.preferences.getBoolean(key, default())
        } else {
            if (persistDefaultIfNotExists) {
                thisRef.preferences.edit { putBoolean(key, default()) }
            }

            default()
        }

    override fun setValue(thisRef: PreferencesHolder, property: KProperty<*>, value: Boolean) =
        thisRef.preferences.edit { putBoolean(key, value) }
}

private class FloatPreference(
    private val key: String,
    private val default: () -> Float,
) : ReadWriteProperty<PreferencesHolder, Float> {

    override fun getValue(thisRef: PreferencesHolder, property: KProperty<*>): Float =
        thisRef.preferences.getFloat(key, default())

    override fun setValue(thisRef: PreferencesHolder, property: KProperty<*>, value: Float) =
        thisRef.preferences.edit { putFloat(key, value) }
}

private class IntPreference(
    private val key: String,
    private val default: () -> Int,
) : ReadWriteProperty<PreferencesHolder, Int> {

    override fun getValue(thisRef: PreferencesHolder, property: KProperty<*>): Int =
        thisRef.preferences.getInt(key, default())

    override fun setValue(thisRef: PreferencesHolder, property: KProperty<*>, value: Int) =
        thisRef.preferences.edit { putInt(key, value) }
}

private class LongPreference(
    private val key: String,
    private val default: () -> Long,
) : ReadWriteProperty<PreferencesHolder, Long> {

    override fun getValue(thisRef: PreferencesHolder, property: KProperty<*>): Long =
        thisRef.preferences.getLong(key, default())

    override fun setValue(thisRef: PreferencesHolder, property: KProperty<*>, value: Long) =
        thisRef.preferences.edit { putLong(key, value) }
}

private class StringPreference(
    private val key: String,
    private val default: () -> String,
    private val persistDefaultIfNotExists: Boolean = false,
) : ReadWriteProperty<PreferencesHolder, String> {

    override fun getValue(thisRef: PreferencesHolder, property: KProperty<*>): String =
        if (thisRef.preferences.contains(key)) {
            thisRef.preferences.getString(key, null) ?: default()
        } else {
            if (persistDefaultIfNotExists) {
                thisRef.preferences.edit { putString(key, default()) }
            }

            default()
        }

    override fun setValue(thisRef: PreferencesHolder, property: KProperty<*>, value: String) =
        thisRef.preferences.edit { putString(key, value) }
}

private class StringSetPreference(
    private val key: String,
    private val default: () -> Set<String>,
) : ReadWriteProperty<PreferencesHolder, Set<String>> {

    override fun getValue(thisRef: PreferencesHolder, property: KProperty<*>): Set<String> =
        thisRef.preferences.getStringSet(key, null) ?: default()

    override fun setValue(thisRef: PreferencesHolder, property: KProperty<*>, value: Set<String>) =
        thisRef.preferences.edit { putStringSet(key, value) }
}

/**
 * Property delegate for getting and setting a boolean shared preference.
 * Optionally this will persist the default value if one is not already persisted.
 *
 * Example usage:
 * ```
 * class Settings : PreferenceHolder {
 *     ...
 *     val isTelemetryOn by booleanPreference(
 *         "telemetry",
 *         default = false,
 *         persistDefaultIfNotExists = true,
 *     )
 * }
 * ```
 */
fun booleanPreference(
    key: String,
    default: Boolean,
    persistDefaultIfNotExists: Boolean = false,
): ReadWriteProperty<PreferencesHolder, Boolean> =
    BooleanPreference(key, { default }, persistDefaultIfNotExists)

/**
 * Property delegate for getting and setting a boolean shared preference.
 * Optionally this will persist the default value if one is not already persisted.
 *
 * The default lambda is not called until the property is read for the first time.
 *
 * Example usage:
 * ```
 * class Settings : PreferenceHolder {
 *     ...
 *     val isTelemetryOn by booleanPreference(
 *         "telemetry",
 *         default = { false },
 *         persistDefaultIfNotExists = true,
 *     )
 * }
 * ```
 */
fun booleanPreference(
    key: String,
    default: () -> Boolean,
    persistDefaultIfNotExists: Boolean = false,
): ReadWriteProperty<PreferencesHolder, Boolean> =
    BooleanPreference(key, default, persistDefaultIfNotExists)

/**
 * Property delegate for getting and setting a float number shared preference.
 *
 * Example usage:
 * ```
 * class Settings : PreferenceHolder {
 *     ...
 *     var percentage by floatPreference(
 *         "percentage",
 *         default = { 0f }
 *     )
 * }
 * ```
 */
fun floatPreference(key: String, default: Float): ReadWriteProperty<PreferencesHolder, Float> =
    FloatPreference(key, { default })

/**
 * Property delegate for getting and setting a float number shared preference.
 *
 * The default lambda is not called until the property is read for the first time.
 *
 * Example usage:
 * ```
 * class Settings : PreferenceHolder {
 *     ...
 *     var percentage by floatPreference(
 *         "percentage",
 *         default = { 0f }
 *     )
 * }
 * ```
 */
fun floatPreference(key: String, default: () -> Float): ReadWriteProperty<PreferencesHolder, Float> =
    FloatPreference(key, default)

/**
 * Property delegate for getting and setting an int number shared preference.
 *
 * Example usage:
 * ```
 * class Settings : PreferenceHolder {
 *     ...
 *     var widgetNumInvocations by intPreference(
 *         "widget_number_of_invocations",
 *         default = 0,
 *      )
 * }
 * ```
 */
fun intPreference(key: String, default: Int): ReadWriteProperty<PreferencesHolder, Int> =
    IntPreference(key, { default })

/**
 * Property delegate for getting and setting an int number shared preference.
 *
 * The default lambda is not called until the property is read for the first time.
 *
 * Example usage:
 * ```
 * class Settings : PreferenceHolder {
 *     ...
 *     var widgetNumInvocations by intPreference(
 *         "widget_number_of_invocations",
 *         default = { 0 },
*      )
 * }
 * ```
 */
fun intPreference(key: String, default: () -> Int): ReadWriteProperty<PreferencesHolder, Int> =
    IntPreference(key, default)

/**
 * Property delegate for getting and setting a long number shared preference.
 *
 * Example usage:
 * ```
 * class Settings : PreferenceHolder {
 *     ...
 *     val appInstanceId by longPreference("app_instance_id", default = 123456789L)
 * }
 * ```
 */
fun longPreference(key: String, default: Long): ReadWriteProperty<PreferencesHolder, Long> =
    LongPreference(key, { default })

/**
 * Property delegate for getting and setting a long number shared preference.
 *
 * The default lambda is not called until the property is read for the first time.
 *
 * Example usage:
 * ```
 * class Settings : PreferenceHolder {
 *     ...
 *     val appInstanceId by longPreference("app_instance_id", default = 123456789L)
 * }
 * ```
 */
fun longPreference(key: String, default: () -> Long): ReadWriteProperty<PreferencesHolder, Long> =
    LongPreference(key, default)

/**
 * Property delegate for getting and setting a string shared preference.
 * Optionally this will persist the default value if one is not already persisted.
 *
 * Example usage:
 * ```
 * class Settings : PreferenceHolder {
 *     ...
 *     var permissionsEnabledEnum by stringPreference(
 *          "permissions_enabled",
 *          default = "blocked",
 *          persistDefaultIfNotExists = true,
 *     )
 * }
 * ```
 */
fun stringPreference(
    key: String,
    default: String,
    persistDefaultIfNotExists: Boolean = false,
): ReadWriteProperty<PreferencesHolder, String> =
    StringPreference(key, { default }, persistDefaultIfNotExists)

/**
 * Property delegate for getting and setting a string shared preference.
 * Optionally this will persist the default value if one is not already persisted.
 *
 * The default lambda is not called until the property is read for the first time.
 *
 * Example usage:
 * ```
 * class Settings : PreferenceHolder {
 *     ...
 *     var permissionsEnabledEnum by stringPreference(
 *          "permissions_enabled",
 *          default = { "blocked" },
 *          persistDefaultIfNotExists = true,
 *     )
 * }
 * ```
 */
fun stringPreference(
    key: String,
    default: () -> String,
    persistDefaultIfNotExists: Boolean = false,
): ReadWriteProperty<PreferencesHolder, String> =
    StringPreference(key, default, persistDefaultIfNotExists)

/**
 * Property delegate for getting and setting a string set shared preference.
 *
 * Example usage:
 * ```
 * class Settings : PreferenceHolder {
 *     ...
 *     var connectedDevices by stringSetPreference("connected_devices", default = emptySet())
 * }
 * ```
 */
fun stringSetPreference(
    key: String,
    default: Set<String>,
): ReadWriteProperty<PreferencesHolder, Set<String>> =
    StringSetPreference(key, { default })

/**
 * Property delegate for getting and setting a string set shared preference.
 *
 * The default lambda is not called until the property is read for the first time.
 *
 * Example usage:
 * ```
 * class Settings : PreferenceHolder {
 *     ...
 *     var connectedDevices by stringSetPreference("connected_devices", default = emptySet())
 * }
 * ```
 */
fun stringSetPreference(
    key: String,
    default: () -> Set<String>,
): ReadWriteProperty<PreferencesHolder, Set<String>> =
    StringSetPreference(key, default)
