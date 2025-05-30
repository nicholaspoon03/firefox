/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

const lazy = {};
const loggersByName = new Map();

ChromeUtils.defineESModuleGetters(lazy, {
  BrowserUtils: "resource://gre/modules/BrowserUtils.sys.mjs",
  Log: "resource://gre/modules/Log.sys.mjs",
  PlacesUIUtils: "moz-src:///browser/components/places/PlacesUIUtils.sys.mjs",
});

ChromeUtils.defineLazyGetter(lazy, "relativeTimeFormat", () => {
  return new Services.intl.RelativeTimeFormat(undefined, { style: "narrow" });
});

// Cutoff of 1.5 minutes + 1 second to determine what text string to display
export const NOW_THRESHOLD_MS = 91000;

// Configure logging level via this pref
export const LOGGING_PREF = "browser.tabs.firefox-view.logLevel";

export const MAX_TABS_FOR_RECENT_BROWSING = 5;

export function formatURIForDisplay(uriString) {
  return lazy.BrowserUtils.formatURIStringForDisplay(uriString);
}

export function convertTimestamp(
  timestamp,
  fluentStrings,
  _nowThresholdMs = NOW_THRESHOLD_MS
) {
  if (!timestamp) {
    // It's marginally better to show nothing instead of "53 years ago"
    return "";
  }
  const elapsed = Date.now() - timestamp;
  let formattedTime;
  if (elapsed <= _nowThresholdMs) {
    // Use a different string for very recent timestamps
    formattedTime = fluentStrings.formatValueSync(
      "firefoxview-just-now-timestamp"
    );
  } else {
    formattedTime = lazy.relativeTimeFormat.formatBestUnit(new Date(timestamp));
  }
  return formattedTime;
}

export function createFaviconElement(image, targetURI = "") {
  let favicon = document.createElement("div");
  favicon.style.backgroundImage = `url('${getImageUrl(image, targetURI)}')`;
  favicon.classList.add("favicon");
  return favicon;
}

export function getImageUrl(icon, targetURI) {
  return icon ? lazy.PlacesUIUtils.getImageURL(icon) : `page-icon:${targetURI}`;
}

/**
 * Get or create a logger, whose log-level is controlled by a pref
 *
 * @param {string} loggerName - Creating named loggers helps differentiate log messages from different
                                components or features.
 */

export function getLogger(loggerName) {
  if (!loggersByName.has(loggerName)) {
    let logger = lazy.Log.repository.getLogger(`FirefoxView.${loggerName}`);
    logger.manageLevelFromPref(LOGGING_PREF);
    logger.addAppender(
      new lazy.Log.ConsoleAppender(new lazy.Log.BasicFormatter())
    );
    loggersByName.set(loggerName, logger);
  }
  return loggersByName.get(loggerName);
}

export function escapeHtmlEntities(text) {
  return (text || "")
    .replace(/&/g, "&amp;")
    .replace(/</g, "&lt;")
    .replace(/>/g, "&gt;")
    .replace(/"/g, "&quot;")
    .replace(/'/g, "&#39;");
}

export function navigateToLink(e, url = e.originalTarget.url) {
  let currentWindow =
    e.target.ownerGlobal.browsingContext.embedderWindowGlobal.browsingContext
      .window;
  if (currentWindow.openTrustedLinkIn) {
    let where = lazy.BrowserUtils.whereToOpenLink(
      e.detail.originalEvent,
      false,
      true
    );
    if (where == "current") {
      where = "tab";
    }
    currentWindow.openTrustedLinkIn(url, where);
  }
}
