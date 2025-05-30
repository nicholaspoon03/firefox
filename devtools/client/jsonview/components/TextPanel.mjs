/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

import { Component } from "resource://devtools/client/shared/vendor/react.mjs";
import PropTypes from "resource://devtools/client/shared/vendor/react-prop-types.mjs";
import * as dom from "resource://devtools/client/shared/vendor/react-dom-factories.mjs";
import { createFactories } from "resource://devtools/client/shared/react-utils.mjs";

import TextToolbarClass from "resource://devtools/client/jsonview/components/TextToolbar.mjs";

const { TextToolbar } = createFactories(TextToolbarClass);
import LiveTextClass from "resource://devtools/client/jsonview/components/LiveText.mjs";

const { LiveText } = createFactories(LiveTextClass);
const { div } = dom;

/**
 * This template represents the 'Raw Data' panel displaying
 * JSON as a text received from the server.
 */
class TextPanel extends Component {
  static get propTypes() {
    return {
      isValidJson: PropTypes.bool,
      actions: PropTypes.object,
      errorMessage: PropTypes.string,
      data: PropTypes.instanceOf(Text),
    };
  }

  constructor(props) {
    super(props);
    this.state = {};
  }

  render() {
    return div(
      { className: "textPanelBox tab-panel-inner" },
      TextToolbar({
        actions: this.props.actions,
        isValidJson: this.props.isValidJson,
      }),
      this.props.errorMessage
        ? div({ className: "jsonParseError" }, this.props.errorMessage)
        : null,
      div({ className: "panelContent" }, LiveText({ data: this.props.data }))
    );
  }
}

export default { TextPanel };
