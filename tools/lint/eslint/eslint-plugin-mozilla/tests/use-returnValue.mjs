/* Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/ */

// ------------------------------------------------------------------------------
// Requirements
// ------------------------------------------------------------------------------

import rule from "../lib/rules/use-returnValue.mjs";
import { RuleTester } from "eslint";

const ruleTester = new RuleTester();

// ------------------------------------------------------------------------------
// Tests
// ------------------------------------------------------------------------------

function invalidCode(code, methodName) {
  return {
    code,
    errors: [
      {
        messageId: "useReturnValue",
        data: {
          property: methodName,
        },
        type: "ExpressionStatement",
      },
    ],
  };
}

ruleTester.run("use-returnValue", rule, {
  valid: [
    "a = foo.concat(bar)",
    "b = bar.concat([1,3,4])",
    "c = baz.concat()",
    "d = qux.join(' ')",
    "e = quux.slice(1)",
  ],
  invalid: [
    invalidCode("foo.concat(bar)", "concat"),
    invalidCode("bar.concat([1,3,4])", "concat"),
    invalidCode("baz.concat()", "concat"),
    invalidCode("qux.join(' ')", "join"),
    invalidCode("quux.slice(1)", "slice"),
  ],
});
