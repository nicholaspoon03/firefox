// |reftest| shell-option(--enable-temporal) skip-if(!this.hasOwnProperty('Temporal')||!xulRuntime.shell) -- Temporal is not enabled unconditionally, requires shell-options
// Copyright (C) 2024 Igalia, S.L. All rights reserved.
// This code is governed by the BSD license found in the LICENSE file.

/*---
esid: sec-temporal.plaindate.prototype.since
description: ISO strings at the edges of the representable range
features: [Temporal]
---*/

const instance = new Temporal.PlainDate(2000, 5, 2);

const validStrings = [
  "-271821-04-19",
  "-271821-04-19T01:00",
  "+275760-09-13",
  "+275760-09-13T23:00",
];

for (const arg of validStrings) {
  instance.since(arg);
}

const invalidStrings = [
  "-271821-04-18",
  "-271821-04-18T23:00",
  "+275760-09-14",
  "+275760-09-14T01:00",
];

for (const arg of invalidStrings) {
  assert.throws(
    RangeError,
    () => instance.since(arg),
    `"${arg}" is outside the representable range of PlainDate`
  );
}

reportCompare(0, 0);
