/* Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/ */

// ------------------------------------------------------------------------------
// Requirements
// ------------------------------------------------------------------------------

import rule from "../lib/rules/reject-lazy-imports-into-globals.mjs";
import { RuleTester } from "eslint";

const ruleTester = new RuleTester();

// ------------------------------------------------------------------------------
// Tests
// ------------------------------------------------------------------------------

function invalidCode(code) {
  return { code, errors: [{ messageId: "rejectLazyImportsIntoGlobals" }] };
}

ruleTester.run("reject-lazy-imports-into-globals", rule, {
  valid: [
    `
      const lazy = {};
      ChromeUtils.defineLazyGetter(lazy, "foo", () => {});
    `,
  ],
  invalid: [
    invalidCode(`ChromeUtils.defineLazyGetter(globalThis, "foo", () => {});`),
    invalidCode(`ChromeUtils.defineLazyGetter(window, "foo", () => {});`),
    invalidCode(
      `XPCOMUtils.defineLazyPreferenceGetter(globalThis, "foo", "foo.bar");`
    ),
    invalidCode(
      `XPCOMUtils.defineLazyServiceGetter(globalThis, "foo", "@foo", "nsIFoo");`
    ),
    invalidCode(`XPCOMUtils.defineLazyGlobalGetters(globalThis, {});`),
    invalidCode(`XPCOMUtils.defineLazyGlobalGetters(window, {});`),
    invalidCode(`ChromeUtils.defineESModuleGetters(globalThis, {});`),
    invalidCode(`ChromeUtils.defineESModuleGetters(window, {});`),
  ],
});
