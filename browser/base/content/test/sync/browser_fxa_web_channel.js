/* Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/
 */

ChromeUtils.defineESModuleGetters(this, {
  WebChannel: "resource://gre/modules/WebChannel.sys.mjs",
  ON_PROFILE_CHANGE_NOTIFICATION:
    "resource://gre/modules/FxAccountsCommon.sys.mjs",
});

var { FxAccountsWebChannel } = ChromeUtils.importESModule(
  "resource://gre/modules/FxAccountsWebChannel.sys.mjs"
);

const TEST_HTTP_PATH = "https://example.com";
const TEST_BASE_URL =
  TEST_HTTP_PATH +
  "/browser/browser/base/content/test/sync/browser_fxa_web_channel.html";
const TEST_CHANNEL_ID = "account_updates_test";

var gTests = [
  {
    desc: "FxA Web Channel - should receive message about profile changes",
    async run() {
      let client = new FxAccountsWebChannel({
        content_uri: TEST_HTTP_PATH,
        channel_id: TEST_CHANNEL_ID,
      });
      let promiseObserver = new Promise(resolve => {
        makeObserver(
          ON_PROFILE_CHANGE_NOTIFICATION,
          function (subject, topic, data) {
            Assert.equal(data, "abc123");
            client.tearDown();
            resolve();
          }
        );
      });

      await BrowserTestUtils.withNewTab(
        {
          gBrowser,
          url: TEST_BASE_URL + "?profile_change",
        },
        async function () {
          await promiseObserver;
        }
      );
    },
  },
  {
    desc: "fxa web channel - login messages should notify the fxAccounts object",
    async run() {
      let promiseLogin = new Promise(resolve => {
        let login = accountData => {
          Assert.equal(typeof accountData.authAt, "number");
          Assert.equal(accountData.email, "testuser@testuser.com");
          Assert.equal(accountData.keyFetchToken, "key_fetch_token");
          Assert.equal(accountData.sessionToken, "session_token");
          Assert.equal(accountData.uid, "uid");
          Assert.equal(accountData.unwrapBKey, "unwrap_b_key");
          Assert.equal(accountData.verified, true);
        };

        let client = new FxAccountsWebChannel({
          content_uri: TEST_HTTP_PATH,
          channel_id: TEST_CHANNEL_ID,
          helpers: {
            login,
          },
        });

        client._channel.send = (message, _context) => {
          Assert.equal(message.data.ok, true);
          client.tearDown();
          resolve();
        };
      });

      await BrowserTestUtils.withNewTab(
        {
          gBrowser,
          url: TEST_BASE_URL + "?login",
        },
        async function () {
          await promiseLogin;
        }
      );
    },
  },
  {
    desc: "fxa web channel - can_link_account messages should respond",
    async run() {
      let properUrl = TEST_BASE_URL + "?can_link_account";

      let promiseEcho = new Promise(resolve => {
        let webChannelOrigin = Services.io.newURI(properUrl);
        // responses sent to content are echoed back over the
        // `fxaccounts_webchannel_response_echo` channel. Ensure the
        // fxaccounts:can_link_account message is responded to.
        let echoWebChannel = new WebChannel(
          "fxaccounts_webchannel_response_echo",
          webChannelOrigin
        );
        echoWebChannel.listen((webChannelId, message) => {
          Assert.equal(message.command, "fxaccounts:can_link_account");
          Assert.equal(message.messageId, 2);
          Assert.equal(message.data.ok, true);

          client.tearDown();
          echoWebChannel.stopListening();

          resolve();
        });

        let client = new FxAccountsWebChannel({
          content_uri: TEST_HTTP_PATH,
          channel_id: TEST_CHANNEL_ID,
          helpers: {
            shouldAllowRelink(acctName) {
              return acctName === "testuser@testuser.com";
            },
            promptProfileSyncWarningIfNeeded(acctName) {
              if (acctName === "testuser@testuser.com") {
                return { action: "continue" };
              }
              return { action: "cancel" };
            },
          },
        });
      });

      await BrowserTestUtils.withNewTab(
        {
          gBrowser,
          url: properUrl,
        },
        async function () {
          await promiseEcho;
        }
      );
    },
  },
  {
    desc: "fxa web channel - logout messages should notify the fxAccounts object",
    async run() {
      let promiseLogout = new Promise(resolve => {
        let logout = uid => {
          Assert.equal(uid, "uid");
        };

        let client = new FxAccountsWebChannel({
          content_uri: TEST_HTTP_PATH,
          channel_id: TEST_CHANNEL_ID,
          helpers: {
            logout,
          },
        });

        client._channel.send = (message, _context) => {
          Assert.equal(message.data.ok, true);
          client.tearDown();
          resolve();
        };
      });

      await BrowserTestUtils.withNewTab(
        {
          gBrowser,
          url: TEST_BASE_URL + "?logout",
        },
        async function () {
          await promiseLogout;
        }
      );
    },
  },
  {
    desc: "fxa web channel - delete messages should notify the fxAccounts object",
    async run() {
      let promiseDelete = new Promise(resolve => {
        let logout = uid => {
          Assert.equal(uid, "uid");
        };

        let client = new FxAccountsWebChannel({
          content_uri: TEST_HTTP_PATH,
          channel_id: TEST_CHANNEL_ID,
          helpers: {
            logout,
          },
        });

        client._channel.send = (message, _context) => {
          Assert.equal(message.data.ok, true);
          client.tearDown();
          resolve();
        };
      });

      await BrowserTestUtils.withNewTab(
        {
          gBrowser,
          url: TEST_BASE_URL + "?delete",
        },
        async function () {
          await promiseDelete;
        }
      );
    },
  },
  {
    desc: "fxa web channel - firefox_view messages should call the openFirefoxView helper",
    async run() {
      let wasCalled = false;
      let promiseMessageHandled = new Promise(resolve => {
        let openFirefoxView = browser => {
          wasCalled = true;
          Assert.ok(
            !!browser.ownerGlobal,
            "openFirefoxView called with a browser argument"
          );
          Assert.equal(
            typeof browser.ownerGlobal.FirefoxViewHandler.openTab,
            "function",
            "We can reach the openTab method"
          );
        };

        let client = new FxAccountsWebChannel({
          content_uri: TEST_HTTP_PATH,
          channel_id: TEST_CHANNEL_ID,
          helpers: {
            openFirefoxView,
          },
        });

        client._channel.send = (message, _context) => {
          Assert.equal(message.data.ok, true);
          client.tearDown();
          resolve();
        };
      });

      await BrowserTestUtils.withNewTab(
        {
          gBrowser,
          url: TEST_BASE_URL + "?firefox_view",
        },
        async function () {
          await promiseMessageHandled;
        }
      );
      Assert.ok(wasCalled, "openFirefoxView did get called");
    },
  },
]; // gTests

function makeObserver(aObserveTopic, aObserveFunc) {
  let callback = function (aSubject, aTopic, aData) {
    if (aTopic == aObserveTopic) {
      removeMe();
      aObserveFunc(aSubject, aTopic, aData);
    }
  };

  function removeMe() {
    Services.obs.removeObserver(callback, aObserveTopic);
  }

  Services.obs.addObserver(callback, aObserveTopic);
  return removeMe;
}

registerCleanupFunction(function () {
  Services.prefs.clearUserPref(
    "browser.tabs.remote.separatePrivilegedMozillaWebContentProcess"
  );
});

function test() {
  waitForExplicitFinish();
  Services.prefs.setBoolPref(
    "browser.tabs.remote.separatePrivilegedMozillaWebContentProcess",
    false
  );

  (async function () {
    for (let testCase of gTests) {
      info("Running: " + testCase.desc);
      await testCase.run();
    }
  })().then(finish, ex => {
    Assert.ok(false, "Unexpected Exception: " + ex);
    finish();
  });
}
