/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

"use strict";

const {
  connect,
} = require("resource://devtools/client/shared/vendor/react-redux.js");
const {
  Component,
  createFactory,
} = require("resource://devtools/client/shared/vendor/react.mjs");
const dom = require("resource://devtools/client/shared/vendor/react-dom-factories.js");
const PropTypes = require("resource://devtools/client/shared/vendor/react-prop-types.mjs");

const AnimationTarget = createFactory(
  require("resource://devtools/client/inspector/animation/components/AnimationTarget.js")
);
const SummaryGraph = createFactory(
  require("resource://devtools/client/inspector/animation/components/graph/SummaryGraph.js")
);

class AnimationItem extends Component {
  static get propTypes() {
    return {
      animation: PropTypes.object.isRequired,
      dispatch: PropTypes.func.isRequired,
      getAnimatedPropertyMap: PropTypes.func.isRequired,
      getNodeFromActor: PropTypes.func.isRequired,
      isDisplayable: PropTypes.bool.isRequired,
      selectAnimation: PropTypes.func.isRequired,
      selectedAnimation: PropTypes.object.isRequired,
      setHighlightedNode: PropTypes.func.isRequired,
      setSelectedNode: PropTypes.func.isRequired,
      simulateAnimation: PropTypes.func.isRequired,
      timeScale: PropTypes.object.isRequired,
    };
  }

  shouldComponentUpdate(nextProps) {
    return (
      this.props.isDisplayable !== nextProps.isDisplayable ||
      this.props.animation !== nextProps.animation ||
      this.props.timeScale !== nextProps.timeScale ||
      this.isSelected(this.props) !== this.isSelected(nextProps)
    );
  }

  isSelected(props) {
    return (
      props.selectedAnimation &&
      props.animation.actorID === props.selectedAnimation.actorID
    );
  }

  render() {
    const {
      animation,
      dispatch,
      getAnimatedPropertyMap,
      getNodeFromActor,
      isDisplayable,
      selectAnimation,
      setHighlightedNode,
      setSelectedNode,
      simulateAnimation,
      timeScale,
    } = this.props;
    const isSelected = this.isSelected(this.props);

    return dom.li(
      {
        className:
          `animation-item ${animation.state.type} ` +
          (isSelected ? "selected" : ""),
      },
      isDisplayable
        ? [
            AnimationTarget({
              animation,
              dispatch,
              getNodeFromActor,
              setHighlightedNode,
              setSelectedNode,
            }),
            SummaryGraph({
              animation,
              getAnimatedPropertyMap,
              selectAnimation,
              simulateAnimation,
              timeScale,
            }),
          ]
        : null
    );
  }
}

const mapStateToProps = state => {
  return {
    selectedAnimation: state.animations.selectedAnimation,
  };
};

module.exports = connect(mapStateToProps)(AnimationItem);
