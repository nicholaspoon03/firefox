/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package mozilla.components.feature.pwa.db

import androidx.core.database.getStringOrNull
import androidx.room.testing.MigrationTestHelper
import androidx.test.platform.app.InstrumentationRegistry
import org.junit.Assert.assertEquals
import org.junit.Assert.assertNull
import org.junit.Assert.assertTrue
import org.junit.Rule
import org.junit.Test
import java.io.IOException

class ManifestDatabaseMigrationTest {
    private val testDbName = "migration-test"

    @Rule
    @JvmField
    val helper: MigrationTestHelper = MigrationTestHelper(
        InstrumentationRegistry.getInstrumentation(),
        ManifestDatabase::class.java,
    )

    @Test
    @Throws(IOException::class)
    fun migrate2To3() {
        helper.createDatabase(testDbName, 2).apply {
            // db has schema version 2. insert some data using SQL queries.
            // You cannot use DAO classes because they expect the latest schema.
            execSQL("INSERT INTO manifests (start_url, created_at, updated_at, manifest, used_at, scope) VALUES ('https://mozilla.org', 1, 2, '{}', 3, 'https://mozilla.org')")

            // Prepare for the next version.
            close()
        }

        // Re-open the database with version 2 and provide
        // MIGRATION_1_2 as the migration process.
        helper.runMigrationsAndValidate(testDbName, 3, true, ManifestDatabase.MIGRATION_2_3).apply {
            val result = query("SELECT scope, has_share_targets FROM manifests WHERE start_url = 'https://mozilla.org'")

            result.moveToNext()

            assertEquals(1, result.count)
            assertTrue(result.isFirst)
            assertTrue(result.isLast)
            assertEquals("https://mozilla.org", result.getStringOrNull(0))
            assertEquals(0, result.getInt(1))

            close()
        }
    }

    @Test
    @Throws(IOException::class)
    fun migrate1To2() {
        helper.createDatabase(testDbName, 1).apply {
            // db has schema version 1. insert some data using SQL queries.
            // You cannot use DAO classes because they expect the latest schema.
            execSQL("INSERT INTO manifests (start_url, created_at, updated_at, manifest, used_at, scope) VALUES ('https://mozilla.org', 1, 2, '{}', 3, 'https://mozilla.org')")

            // Prepare for the next version.
            close()
        }

        // Re-open the database with version 2 and provide
        // MIGRATION_1_2 as the migration process.
        helper.runMigrationsAndValidate(testDbName, 2, true, ManifestDatabase.MIGRATION_1_2).apply {
            val result = query("SELECT scope, used_at FROM manifests WHERE start_url = 'https://mozilla.org'")

            result.moveToNext()

            assertEquals(1, result.count)
            assertTrue(result.isFirst)
            assertTrue(result.isLast)
            assertEquals("https://mozilla.org", result.getStringOrNull(0))
            assertEquals(3, result.getLong(1))

            close()
        }
    }

    @Test
    @Throws(IOException::class)
    fun migrate0To2() {
        helper.createDatabase(testDbName, 0).apply {
            // db has schema version 0 which was the original version 1. insert some data using SQL queries.
            // You cannot use DAO classes because they expect the latest schema.
            execSQL("INSERT INTO manifests (start_url, created_at, updated_at, manifest) VALUES ('https://mozilla.org', 1, 2, '{}')")

            // Prepare for the next version.
            close()
        }

        // Re-open the database with version 2 and provide
        // MIGRATION_1_2 as the migration process.
        helper.runMigrationsAndValidate(testDbName, 2, true, ManifestDatabase.MIGRATION_1_2).apply {
            val result = query("SELECT scope, used_at FROM manifests WHERE start_url = 'https://mozilla.org'")

            result.moveToNext()

            assertEquals(1, result.count)
            assertTrue(result.isFirst)
            assertTrue(result.isLast)
            assertNull(result.getStringOrNull(0))
            assertEquals(2, result.getLong(1))

            close()
        }
    }
}
