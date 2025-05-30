/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package mozilla.components.support.utils

import androidx.core.text.TextDirectionHeuristicCompat
import androidx.test.ext.junit.runners.AndroidJUnit4
import mozilla.components.support.ktx.util.URLStringUtils
import mozilla.components.support.ktx.util.URLStringUtils.isSearchTerm
import mozilla.components.support.ktx.util.URLStringUtils.isURLLike
import mozilla.components.support.ktx.util.URLStringUtils.toNormalizedURL
import org.junit.Assert.assertEquals
import org.junit.Assert.assertFalse
import org.junit.Assert.assertTrue
import org.junit.Before
import org.junit.Test
import org.junit.runner.RunWith
import kotlin.random.Random

@RunWith(AndroidJUnit4::class)
class URLStringUtilsTest {

    @Before
    fun configurePatternFlags() {
        URLStringUtils.flags = URLStringUtils.UNICODE_CHARACTER_CLASS
    }

    @Test
    fun toNormalizedURL() {
        val expectedUrl = "http://mozilla.org"
        assertEquals(expectedUrl, toNormalizedURL("http://mozilla.org"))
        assertEquals(expectedUrl, toNormalizedURL("  http://mozilla.org  "))
        assertEquals(expectedUrl, toNormalizedURL("mozilla.org"))
        assertEquals(expectedUrl, toNormalizedURL("HTTP://mozilla.org"))
        assertEquals("file:///mnt/sdcard/", toNormalizedURL("file:///mnt/sdcard/"))
        assertEquals("http://mozilla.org", toNormalizedURL(" http://mozilla.org"))
        assertEquals("http://localhost", toNormalizedURL("localhost"))

        assertEquals(
            "https://www.mozilla.org/en-US/internet-health/",
            toNormalizedURL("https://www.mozilla.org/en-US/internet-health/"),
        )
    }

    @Test
    fun isUrlLike() {
        assertFalse(isURLLike("inurl:mozilla.org advanced search"))
        assertFalse(isURLLike("sf: help"))
        assertFalse(isURLLike("mozilla./~"))
        assertFalse(isURLLike("cnn.com politics"))

        assertTrue(isURLLike("about:config"))
        assertTrue(isURLLike("about:config:8000"))
        assertTrue(isURLLike("file:///home/user/myfile.html"))
        assertTrue(isURLLike("file://////////////home//user/myfile.html"))
        assertTrue(isURLLike("file://C:\\Users\\user\\myfile.html"))
        assertTrue(isURLLike("http://192.168.255.255"))
        assertTrue(isURLLike("link.unknown"))
        // Per https://bugs.chromium.org/p/chromium/issues/detail?id=31405, ICANN will accept
        // purely numeric gTLDs.
        assertTrue(isURLLike("3.14.2019"))
        assertTrue(isURLLike("3-four.14.2019"))
        assertTrue(isURLLike(" cnn.com "))
        assertTrue(isURLLike(" cnn.com"))
        assertTrue(isURLLike("cnn.com "))
        assertTrue(isURLLike("mozilla.com/~userdir"))
        assertTrue(isURLLike("my-domain.com"))
        assertTrue(isURLLike("http://faß.de//"))
        assertTrue(isURLLike("cnn.cơḿ"))
        assertTrue(isURLLike("cnn.çơḿ"))

        // Examples from the code comments:
        assertTrue(isURLLike("c-c.com"))
        assertTrue(isURLLike("c-c-c-c.c-c-c"))
        assertTrue(isURLLike("c-http://c.com"))
        assertTrue(isURLLike("about-mozilla:mozilla"))
        assertTrue(isURLLike("c-http.d-x"))
        assertTrue(isURLLike("www.c.-"))
        assertTrue(isURLLike("3-3.3"))
        assertTrue(isURLLike("www.c-c.-"))

        assertFalse(isURLLike(" -://x.com "))
        assertFalse(isURLLike("  -x.com"))
        assertFalse(isURLLike("http://www-.com"))
        assertFalse(isURLLike("www.c-c-  "))
        assertFalse(isURLLike("3-3 "))

        // Valid IPv6 literals correctly recognized as valid.
        val validIPv6Literals = listOf(
            "[::]",
            "[::1]",
            "[1::]",
            "[1:2:3:4:5:6:7:8]",
            "[2001:db8::1.2.3.4]",
            "[::1]:8080",
        )

        validIPv6Literals.forEach { url ->
            assertTrue(isURLLike(url))
            assertTrue(isURLLike("$url/"))
            assertTrue(isURLLike("https://$url"))
            assertTrue(isURLLike("https://$url/"))
            assertTrue(isURLLike("https:$url"))
            assertTrue(isURLLike("https:$url/"))
            assertTrue(isURLLike("http://$url"))
            assertTrue(isURLLike("http://$url/"))
            assertTrue(isURLLike("http:$url"))
            assertTrue(isURLLike("http:$url/"))
        }

        // Invalid IPv6 literals correctly recognized as invalid.
        assertFalse(isURLLike("::1"))
        assertFalse(isURLLike(":::"))
        assertFalse(isURLLike("[[http://]]"))
        assertFalse(isURLLike("[[["))
        assertFalse(isURLLike("[[[:"))
        assertFalse(isURLLike("[[[:/"))
        assertFalse(isURLLike("http://]]]"))

        // Invalid IPv6 literals correctly recognized as something else.
        assertTrue(isURLLike("fe80::"))
        assertTrue(isURLLike("x:["))

        // Invalid IPv6 literals incorrectly recognized as valid.
        // We allow these for now, until bug 1685152 is fixed.
        assertTrue(isURLLike("[:::"))
        assertTrue(isURLLike("http://[::"))
        assertTrue(isURLLike("http://[::/path"))
        assertTrue(isURLLike("http://[::?query"))
        assertTrue(isURLLike("[[http://banana]]"))
        assertTrue(isURLLike("http://[[["))
        assertTrue(isURLLike("[[[::"))
        assertTrue(isURLLike("[[[::/"))
        assertTrue(isURLLike("http://[1.2.3]"))
        assertTrue(isURLLike("https://[1:2:3:4:5:6:7]/"))
        assertTrue(isURLLike("https://[1:2:3:4:5:6:7:8:9]/"))

        // Examples from issues
        assertTrue(isURLLike("https://abc--cba.com/")) // #7096
    }

    @Test
    fun isSearchTerm() {
        assertTrue(isSearchTerm("inurl:mozilla.org advanced search"))
        assertTrue(isSearchTerm("sf: help"))
        assertTrue(isSearchTerm("mozilla./~"))
        assertTrue(isSearchTerm("cnn.com politics"))

        assertFalse(isSearchTerm("about:config"))
        assertFalse(isSearchTerm("about:config:8000"))
        assertFalse(isSearchTerm("file:///home/user/myfile.html"))
        assertFalse(isSearchTerm("file://////////////home//user/myfile.html"))
        assertFalse(isSearchTerm("file://C:\\Users\\user\\myfile.html"))
        assertFalse(isSearchTerm("http://192.168.255.255"))
        assertFalse(isSearchTerm("link.unknown"))
        // Per https://bugs.chromium.org/p/chromium/issues/detail?id=31405, ICANN will accept
        // purely numeric gTLDs.
        assertFalse(isSearchTerm("3.14.2019"))
        assertFalse(isSearchTerm("3-four.14.2019"))
        assertFalse(isSearchTerm(" cnn.com "))
        assertFalse(isSearchTerm(" cnn.com"))
        assertFalse(isSearchTerm("cnn.com "))
        assertFalse(isSearchTerm("my-domain.com"))
        assertFalse(isSearchTerm("camp-firefox.de"))
        assertFalse(isSearchTerm("http://my-domain.com"))
        assertFalse(isSearchTerm("mozilla.com/~userdir"))
        assertFalse(isSearchTerm("http://faß.de//"))
        assertFalse(isSearchTerm("cnn.cơḿ"))
        assertFalse(isSearchTerm("cnn.çơḿ"))

        // Examples from the code comments:
        assertFalse(isSearchTerm("c-c.com"))
        assertFalse(isSearchTerm("c-c-c-c.c-c-c"))
        assertFalse(isSearchTerm("c-http://c.com"))
        assertFalse(isSearchTerm("about-mozilla:mozilla"))
        assertFalse(isSearchTerm("c-http.d-x"))
        assertFalse(isSearchTerm("www.c.-"))
        assertFalse(isSearchTerm("3-3.3"))
        assertFalse(isSearchTerm("www.c-c.-"))

        assertTrue(isSearchTerm(" -://x.com "))
        assertTrue(isSearchTerm("  -x.com"))
        assertTrue(isSearchTerm("http://www-.com"))
        assertTrue(isSearchTerm("www.c-c-  "))
        assertTrue(isSearchTerm("3-3 "))

        // Examples from issues
        assertFalse(isSearchTerm("https://abc--cba.com/")) // #7096
    }

    @Test
    fun stripUrlSchemeUrlWithHttps() {
        val testDisplayUrl = URLStringUtils.toDisplayUrl("https://mozilla.com")
        assertEquals("mozilla.com", testDisplayUrl)
    }

    @Test
    fun stripTrailingSlash() {
        val testDisplayUrl = URLStringUtils.toDisplayUrl("mozilla.com/")
        assertEquals("mozilla.com", testDisplayUrl)
    }

    @Test
    fun stripUrlSchemeUrlWithHttpsAndTrailingSlash() {
        val testDisplayUrl = URLStringUtils.toDisplayUrl("https://mozilla.com/")
        assertEquals("mozilla.com", testDisplayUrl)
    }

    @Test
    fun stripUrlSchemeUrlWithHttp() {
        val testDisplayUrl = URLStringUtils.toDisplayUrl("http://mozilla.com")
        assertEquals("mozilla.com", testDisplayUrl)
    }

    @Test
    fun stripUrlSubdomainUrlWithHttps() {
        val testDisplayUrl = URLStringUtils.toDisplayUrl("https://www.mozilla.com")
        assertEquals("mozilla.com", testDisplayUrl)
    }

    @Test
    fun stripUrlSubdomainUrlWithHttp() {
        val testDisplayUrl = URLStringUtils.toDisplayUrl("http://www.mozilla.com")
        assertEquals("mozilla.com", testDisplayUrl)
    }

    @Test
    fun stripUrlSchemeUrlWithHttpsFollowedByHttp() {
        val testDisplayUrl = URLStringUtils.toDisplayUrl("https://http://mozilla.com")
        assertEquals("http://mozilla.com", testDisplayUrl)
    }

    @Test
    fun stripUrlSchemeUrlWithHttpFollowedByHttps() {
        val testDisplayUrl = URLStringUtils.toDisplayUrl("http://https://mozilla.com")
        assertEquals("https://mozilla.com", testDisplayUrl)
    }

    @Test
    fun stripUrlSchemeAndSubdomainUrlNoMatch() {
        val testDisplayUrl = URLStringUtils.toDisplayUrl("zzz://www.mozillahttp://.com")
        assertEquals("zzz://www.mozillahttp://.com", testDisplayUrl)
    }

    @Test
    fun toDisplayUrlDoesNotAddImplicitDirectionalMarks() {
        val testDisplayUrl = URLStringUtils.toDisplayUrl("http://ختار.ار/www.mozilla.org/1")
        assertEquals("ختار.ار/www.mozilla.org/1", testDisplayUrl)
    }

    @Test
    fun toDisplayUrlHandlesBlankStrings() {
        assertEquals("", URLStringUtils.toDisplayUrl(""))

        assertEquals("  ", URLStringUtils.toDisplayUrl("  "))
    }

    @Test
    fun isHttpOrHttpsUrl() {
        assertFalse(URLStringUtils.isHttpOrHttps(""))
        assertFalse(URLStringUtils.isHttpOrHttps("     "))
        assertFalse(URLStringUtils.isHttpOrHttps("mozilla.org"))
        assertFalse(URLStringUtils.isHttpOrHttps("httpstrf://example.org"))
        assertTrue(URLStringUtils.isHttpOrHttps("https://www.mozilla.org"))
        assertTrue(URLStringUtils.isHttpOrHttps("http://example.org"))
        assertTrue(URLStringUtils.isHttpOrHttps("http://192.168.0.1"))
    }

    @Test
    fun isValidSearchQueryUrl() {
        assertTrue(URLStringUtils.isValidSearchQueryUrl("https://example.com/search/?q=%s"))
        assertTrue(URLStringUtils.isValidSearchQueryUrl("http://example.com/search/?q=%s"))
        assertTrue(URLStringUtils.isValidSearchQueryUrl("http-test-site.com/search/?q=%s"))
        assertFalse(URLStringUtils.isValidSearchQueryUrl("httpss://example.com/search/?q=%s"))
        assertTrue(URLStringUtils.isValidSearchQueryUrl("example.com/search/?q=%s"))
        assertTrue(URLStringUtils.isValidSearchQueryUrl(" example.com/search/?q=%s "))
        assertFalse(URLStringUtils.isValidSearchQueryUrl("htps://example.com/search/?q=%s"))
    }
}

/**
 * Custom [TextDirectionHeuristicCompat] used only in tests to make possible testing of RTL checks.
 * Overcomes the limitations not allowing Mockito to mock platform implementations.
 *
 * The return of both [isRtl] is non-deterministic. Setup a different behavior if needed.
 */
private open class TestTextDirectionHeuristicCompat : TextDirectionHeuristicCompat {
    override fun isRtl(array: CharArray?, start: Int, count: Int): Boolean {
        return Random.nextBoolean()
    }

    override fun isRtl(cs: CharSequence?, start: Int, count: Int): Boolean {
        return Random.nextBoolean()
    }
}
