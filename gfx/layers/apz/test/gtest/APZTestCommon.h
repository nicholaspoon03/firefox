/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set ts=8 sts=2 et sw=2 tw=80: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef mozilla_layers_APZTestCommon_h
#define mozilla_layers_APZTestCommon_h

/**
 * Defines a set of mock classes and utility functions/classes for
 * writing APZ gtests.
 */

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "mozilla/Attributes.h"
#include "mozilla/layers/GeckoContentController.h"
#include "mozilla/layers/CompositorBridgeParent.h"
#include "mozilla/layers/DoubleTapToZoom.h"
#include "mozilla/layers/APZThreadUtils.h"
#include "mozilla/layers/MatrixMessage.h"
#include "mozilla/StaticPrefs_layout.h"
#include "mozilla/TypedEnumBits.h"
#include "mozilla/UniquePtr.h"
#include "apz/src/APZCTreeManager.h"
#include "apz/src/AsyncPanZoomController.h"
#include "apz/src/HitTestingTreeNode.h"
#include "base/task.h"
#include "gfxPlatform.h"
#include "TestWRScrollData.h"
#include "UnitTransforms.h"

using namespace mozilla;
using namespace mozilla::gfx;
using namespace mozilla::layers;
using ::testing::_;
using ::testing::AtLeast;
using ::testing::AtMost;
using ::testing::InSequence;
using ::testing::MockFunction;
using ::testing::NiceMock;
typedef mozilla::layers::GeckoContentController::TapType TapType;

inline TimeStamp GetStartupTime() {
  static TimeStamp sStartupTime = TimeStamp::Now();
  return sStartupTime;
}

inline uint32_t MillisecondsSinceStartup(TimeStamp aTime) {
  return (aTime - GetStartupTime()).ToMilliseconds();
}

// Some helper functions for constructing input event objects suitable to be
// passed either to an APZC (which expects an transformed point), or to an APZTM
// (which expects an untransformed point). We handle both cases by setting both
// the transformed and untransformed fields to the same value.
inline SingleTouchData CreateSingleTouchData(int32_t aIdentifier,
                                             const ScreenIntPoint& aPoint) {
  SingleTouchData touch(aIdentifier, aPoint, ScreenSize(0, 0), 0, 0);
  touch.mLocalScreenPoint = ParentLayerPoint(aPoint.x, aPoint.y);
  return touch;
}

// Convenience wrapper for CreateSingleTouchData() that takes loose coordinates.
inline SingleTouchData CreateSingleTouchData(int32_t aIdentifier,
                                             ScreenIntCoord aX,
                                             ScreenIntCoord aY) {
  return CreateSingleTouchData(aIdentifier, ScreenIntPoint(aX, aY));
}

inline PinchGestureInput CreatePinchGestureInput(
    PinchGestureInput::PinchGestureType aType, const ScreenPoint& aFocus,
    float aCurrentSpan, float aPreviousSpan, TimeStamp timestamp) {
  ParentLayerPoint localFocus(aFocus.x, aFocus.y);
  PinchGestureInput result(aType, PinchGestureInput::UNKNOWN, timestamp,
                           ExternalPoint(0, 0), aFocus, aCurrentSpan,
                           aPreviousSpan, 0);
  return result;
}

template <class SetArg, class Storage>
class ScopedGfxSetting {
 public:
  ScopedGfxSetting(const std::function<SetArg(void)>& aGetPrefFunc,
                   const std::function<void(SetArg)>& aSetPrefFunc, SetArg aVal)
      : mSetPrefFunc(aSetPrefFunc) {
    mOldVal = aGetPrefFunc();
    aSetPrefFunc(aVal);
  }

  ~ScopedGfxSetting() { mSetPrefFunc(mOldVal); }

 private:
  std::function<void(SetArg)> mSetPrefFunc;
  Storage mOldVal;
};

static inline constexpr auto kDefaultTouchBehavior =
    AllowedTouchBehavior::VERTICAL_PAN | AllowedTouchBehavior::HORIZONTAL_PAN |
    AllowedTouchBehavior::PINCH_ZOOM | AllowedTouchBehavior::ANIMATING_ZOOM;

#define FRESH_PREF_VAR_PASTE(id, line) id##line
#define FRESH_PREF_VAR_EXPAND(id, line) FRESH_PREF_VAR_PASTE(id, line)
#define FRESH_PREF_VAR FRESH_PREF_VAR_EXPAND(pref, __LINE__)

#define SCOPED_GFX_PREF_BOOL(prefName, prefValue)                           \
  ScopedGfxSetting<bool, bool> FRESH_PREF_VAR(                              \
      [=]() { return Preferences::GetBool(prefName); },                     \
      [=](bool aPrefValue) { Preferences::SetBool(prefName, aPrefValue); }, \
      prefValue)

#define SCOPED_GFX_PREF_INT(prefName, prefValue)                              \
  ScopedGfxSetting<int32_t, int32_t> FRESH_PREF_VAR(                          \
      [=]() { return Preferences::GetInt(prefName); },                        \
      [=](int32_t aPrefValue) { Preferences::SetInt(prefName, aPrefValue); }, \
      prefValue)

#define SCOPED_GFX_PREF_FLOAT(prefName, prefValue)                            \
  ScopedGfxSetting<float, float> FRESH_PREF_VAR(                              \
      [=]() { return Preferences::GetFloat(prefName); },                      \
      [=](float aPrefValue) { Preferences::SetFloat(prefName, aPrefValue); }, \
      prefValue)

class MockContentController : public GeckoContentController {
 public:
  MOCK_METHOD1(NotifyLayerTransforms, void(nsTArray<MatrixMessage>&&));
  MOCK_METHOD1(RequestContentRepaint, void(const RepaintRequest&));
  MOCK_METHOD6(HandleTap, void(TapType, const LayoutDevicePoint&, Modifiers,
                               const ScrollableLayerGuid&, uint64_t,
                               const Maybe<DoubleTapToZoomMetrics>&));
  MOCK_METHOD5(NotifyPinchGesture,
               void(PinchGestureInput::PinchGestureType,
                    const ScrollableLayerGuid&, const LayoutDevicePoint&,
                    LayoutDeviceCoord, Modifiers));
  // Can't use the macros with already_AddRefed :(
  void PostDelayedTask(already_AddRefed<Runnable> aTask, int aDelayMs) {
    RefPtr<Runnable> task = aTask;
  }
  bool IsRepaintThread() { return NS_IsMainThread(); }
  void DispatchToRepaintThread(already_AddRefed<Runnable> aTask) {
    NS_DispatchToMainThread(std::move(aTask));
  }
  MOCK_METHOD4(NotifyAPZStateChange,
               void(const ScrollableLayerGuid& aGuid, APZStateChange aChange,
                    int aArg, Maybe<uint64_t> aInputBlockId));
  MOCK_METHOD0(NotifyFlushComplete, void());
  MOCK_METHOD3(NotifyAsyncScrollbarDragInitiated,
               void(uint64_t, const ScrollableLayerGuid::ViewID&,
                    ScrollDirection aDirection));
  MOCK_METHOD1(NotifyAsyncScrollbarDragRejected,
               void(const ScrollableLayerGuid::ViewID&));
  MOCK_METHOD1(NotifyAsyncAutoscrollRejected,
               void(const ScrollableLayerGuid::ViewID&));
  MOCK_METHOD1(CancelAutoscroll, void(const ScrollableLayerGuid&));
  MOCK_METHOD2(NotifyScaleGestureComplete,
               void(const ScrollableLayerGuid&, float aScale));
  MOCK_METHOD4(UpdateOverscrollVelocity,
               void(const ScrollableLayerGuid&, float, float, bool));
  MOCK_METHOD4(UpdateOverscrollOffset,
               void(const ScrollableLayerGuid&, float, float, bool));
};

class MockContentControllerDelayed : public MockContentController {
 public:
  MockContentControllerDelayed()
      : mTime(SampleTime::FromTest(GetStartupTime())) {}

  const TimeStamp& Time() { return mTime.Time(); }
  const SampleTime& GetSampleTime() { return mTime; }

  void AdvanceByMillis(int aMillis) {
    AdvanceBy(TimeDuration::FromMilliseconds(aMillis));
  }

  void AdvanceBy(const TimeDuration& aIncrement) {
    SampleTime target = mTime + aIncrement;
    while (mTaskQueue.Length() > 0 && mTaskQueue[0].second <= target) {
      RunNextDelayedTask();
    }
    mTime = target;
  }

  void PostDelayedTask(already_AddRefed<Runnable> aTask, int aDelayMs) {
    RefPtr<Runnable> task = aTask;
    SampleTime runAtTime = mTime + TimeDuration::FromMilliseconds(aDelayMs);
    int insIndex = mTaskQueue.Length();
    while (insIndex > 0) {
      if (mTaskQueue[insIndex - 1].second <= runAtTime) {
        break;
      }
      insIndex--;
    }
    mTaskQueue.InsertElementAt(insIndex, std::make_pair(task, runAtTime));
  }

  // Run all the tasks in the queue, returning the number of tasks
  // run. Note that if a task queues another task while running, that
  // new task will not be run. Therefore, there may be still be tasks
  // in the queue after this function is called. Only when the return
  // value is 0 is the queue guaranteed to be empty.
  int RunThroughDelayedTasks() {
    nsTArray<std::pair<RefPtr<Runnable>, SampleTime>> runQueue =
        std::move(mTaskQueue);
    int numTasks = runQueue.Length();
    for (int i = 0; i < numTasks; i++) {
      mTime = runQueue[i].second;
      runQueue[i].first->Run();

      // Deleting the task is important in order to release the reference to
      // the callee object.
      runQueue[i].first = nullptr;
    }
    return numTasks;
  }

 private:
  void RunNextDelayedTask() {
    std::pair<RefPtr<Runnable>, SampleTime> next = mTaskQueue[0];
    mTaskQueue.RemoveElementAt(0);
    mTime = next.second;
    next.first->Run();
    // Deleting the task is important in order to release the reference to
    // the callee object.
    next.first = nullptr;
  }

  // The following array is sorted by timestamp (tasks are inserted in order by
  // timestamp).
  nsTArray<std::pair<RefPtr<Runnable>, SampleTime>> mTaskQueue;
  SampleTime mTime;
};

class TestAPZCTreeManager : public APZCTreeManager {
 public:
  explicit TestAPZCTreeManager(MockContentControllerDelayed* aMcc,
                               UniquePtr<IAPZHitTester> aHitTester = nullptr)
      : APZCTreeManager(LayersId{0}, std::move(aHitTester)), mcc(aMcc) {
    Init();
  }

  RefPtr<InputQueue> GetInputQueue() const { return mInputQueue; }

  void ClearContentController() { mcc = nullptr; }

  /**
   * This function is not currently implemented.
   * See bug 1468804 for more information.
   **/
  void CancelAnimation() { EXPECT_TRUE(false); }

  bool AdvanceAnimations(const SampleTime& aSampleTime) {
    MutexAutoLock lock(mMapLock);
    return AdvanceAnimationsInternal(lock, aSampleTime);
  }

  APZEventResult ReceiveInputEvent(
      InputData& aEvent,
      InputBlockCallback&& aCallback = InputBlockCallback()) override {
    APZEventResult result =
        APZCTreeManager::ReceiveInputEvent(aEvent, std::move(aCallback));
    if (aEvent.mInputType == PANGESTURE_INPUT &&
        // In the APZCTreeManager::ReceiveInputEvent some type of pan gesture
        // events are marked as `mHandledByAPZ = false` (e.g. with Ctrl key
        // modifier which causes reflow zoom), in such cases the events will
        // never be processed by InputQueue so we shouldn't try to invoke
        // AllowsSwipe() here.
        aEvent.AsPanGestureInput().mHandledByAPZ &&
        aEvent.AsPanGestureInput().AllowsSwipe()) {
      SetBrowserGestureResponse(result.mInputBlockId,
                                BrowserGestureResponse::NotConsumed);
    }
    return result;
  }

 protected:
  already_AddRefed<AsyncPanZoomController> NewAPZCInstance(
      LayersId aLayersId, GeckoContentController* aController) override;

  SampleTime GetFrameTime() override { return mcc->GetSampleTime(); }

 private:
  RefPtr<MockContentControllerDelayed> mcc;
};

class TestAsyncPanZoomController : public AsyncPanZoomController {
 public:
  TestAsyncPanZoomController(LayersId aLayersId,
                             MockContentControllerDelayed* aMcc,
                             TestAPZCTreeManager* aTreeManager,
                             GestureBehavior aBehavior = DEFAULT_GESTURES)
      : AsyncPanZoomController(aLayersId, aTreeManager,
                               aTreeManager->GetInputQueue(), aMcc, aBehavior),
        mWaitForMainThread(false),
        mcc(aMcc) {}

  APZEventResult ReceiveInputEvent(
      InputData& aEvent,
      const Maybe<nsTArray<uint32_t>>& aTouchBehaviors = Nothing()) {
    // This is a function whose signature matches exactly the ReceiveInputEvent
    // on APZCTreeManager. This allows us to templates for functions like
    // TouchDown, TouchUp, etc so that we can reuse the code for dispatching
    // events into both APZC and APZCTM.
    APZEventResult result = GetInputQueue()->ReceiveInputEvent(
        this, TargetConfirmationFlags{!mWaitForMainThread}, aEvent,
        aTouchBehaviors);

    if (aEvent.mInputType == PANGESTURE_INPUT &&
        aEvent.AsPanGestureInput().AllowsSwipe()) {
      GetInputQueue()->SetBrowserGestureResponse(
          result.mInputBlockId, BrowserGestureResponse::NotConsumed);
    }
    return result;
  }

  void ContentReceivedInputBlock(uint64_t aInputBlockId, bool aPreventDefault) {
    GetInputQueue()->ContentReceivedInputBlock(aInputBlockId, aPreventDefault);
  }

  void ConfirmTarget(uint64_t aInputBlockId) {
    RefPtr<AsyncPanZoomController> target = this;
    GetInputQueue()->SetConfirmedTargetApzc(aInputBlockId, target);
  }

  void SetAllowedTouchBehavior(uint64_t aInputBlockId,
                               const nsTArray<TouchBehaviorFlags>& aBehaviors) {
    GetInputQueue()->SetAllowedTouchBehavior(aInputBlockId, aBehaviors);
  }

  void SetFrameMetrics(const FrameMetrics& metrics) {
    RecursiveMutexAutoLock lock(mRecursiveMutex);
    Metrics() = metrics;
  }

  void SetScrollMetadata(const ScrollMetadata& aMetadata) {
    RecursiveMutexAutoLock lock(mRecursiveMutex);
    mScrollMetadata = aMetadata;
  }

  FrameMetrics& GetFrameMetrics() {
    RecursiveMutexAutoLock lock(mRecursiveMutex);
    return mScrollMetadata.GetMetrics();
  }

  ScrollMetadata& GetScrollMetadata() {
    RecursiveMutexAutoLock lock(mRecursiveMutex);
    return mScrollMetadata;
  }

  const FrameMetrics& GetFrameMetrics() const {
    RecursiveMutexAutoLock lock(mRecursiveMutex);
    return mScrollMetadata.GetMetrics();
  }

  using AsyncPanZoomController::GetOverscrollAmount;
  using AsyncPanZoomController::GetVelocityVector;

  void AssertStateIsReset() const {
    RecursiveMutexAutoLock lock(mRecursiveMutex);
    EXPECT_EQ(NOTHING, mState);
  }

  void AssertStateIsFling() const {
    RecursiveMutexAutoLock lock(mRecursiveMutex);
    EXPECT_EQ(FLING, mState);
  }

  void AssertStateIsSmoothScroll() const {
    RecursiveMutexAutoLock lock(mRecursiveMutex);
    EXPECT_EQ(SMOOTH_SCROLL, mState);
  }

  void AssertStateIsSmoothMsdScroll() const {
    RecursiveMutexAutoLock lock(mRecursiveMutex);
    EXPECT_EQ(SMOOTHMSD_SCROLL, mState);
  }

  void AssertStateIsPanningLockedY() {
    RecursiveMutexAutoLock lock(mRecursiveMutex);
    EXPECT_EQ(PANNING_LOCKED_Y, mState);
  }

  void AssertStateIsPanningLockedX() {
    RecursiveMutexAutoLock lock(mRecursiveMutex);
    EXPECT_EQ(PANNING_LOCKED_X, mState);
  }

  void AssertStateIsPanning() {
    RecursiveMutexAutoLock lock(mRecursiveMutex);
    EXPECT_EQ(PANNING, mState);
  }

  void AssertStateIsPanMomentum() {
    RecursiveMutexAutoLock lock(mRecursiveMutex);
    EXPECT_EQ(PAN_MOMENTUM, mState);
  }

  void AssertStateIsWheelScroll() {
    RecursiveMutexAutoLock lock(mRecursiveMutex);
    EXPECT_EQ(WHEEL_SCROLL, mState);
  }

  void AssertStateIsAutoscroll() {
    RecursiveMutexAutoLock lock(mRecursiveMutex);
    EXPECT_EQ(AUTOSCROLL, mState);
  }

  void SetAxisLocked(ScrollDirections aDirections, bool aLockValue) {
    if (aDirections.contains(ScrollDirection::eVertical)) {
      mY.SetAxisLocked(aLockValue);
    }
    if (aDirections.contains(ScrollDirection::eHorizontal)) {
      mX.SetAxisLocked(aLockValue);
    }
  }

  void AssertNotAxisLocked() const {
    EXPECT_FALSE(mY.IsAxisLocked());
    EXPECT_FALSE(mX.IsAxisLocked());
  }

  void AssertAxisLocked(ScrollDirection aDirection) const {
    switch (aDirection) {
      case ScrollDirection::eHorizontal:
        EXPECT_TRUE(mY.IsAxisLocked());
        EXPECT_FALSE(mX.IsAxisLocked());
        break;
      case ScrollDirection::eVertical:
        EXPECT_TRUE(mX.IsAxisLocked());
        EXPECT_FALSE(mY.IsAxisLocked());
        break;
      default:
        FAIL() << "input direction must be either vertical or horizontal";
    }
  }

  void AdvanceAnimationsUntilEnd(
      const TimeDuration& aIncrement = TimeDuration::FromMilliseconds(10)) {
    while (AdvanceAnimations(mcc->GetSampleTime())) {
      mcc->AdvanceBy(aIncrement);
    }
  }

  bool SampleContentTransformForFrame(
      AsyncTransform* aOutTransform, ParentLayerPoint& aScrollOffset,
      const TimeDuration& aIncrement = TimeDuration::FromMilliseconds(0)) {
    mcc->AdvanceBy(aIncrement);
    bool ret = AdvanceAnimations(mcc->GetSampleTime());
    if (aOutTransform) {
      *aOutTransform =
          GetCurrentAsyncTransform(AsyncPanZoomController::eForEventHandling);
    }
    aScrollOffset =
        GetCurrentAsyncScrollOffset(AsyncPanZoomController::eForEventHandling);
    return ret;
  }

  CSSPoint GetCompositedScrollOffset() const {
    return GetCurrentAsyncScrollOffset(
               AsyncPanZoomController::eForCompositing) /
           GetFrameMetrics().GetZoom();
  }

  void SetWaitForMainThread() { mWaitForMainThread = true; }

  bool IsOverscrollAnimationRunning() const {
    return mState == PanZoomState::OVERSCROLL_ANIMATION;
  }

  bool IsWheelScrollAnimationRunning() const {
    return mState == PanZoomState::WHEEL_SCROLL;
  }

 private:
  bool mWaitForMainThread;
  MockContentControllerDelayed* mcc;
};

class APZCTesterBase : public ::testing::Test {
 public:
  APZCTesterBase() { mcc = new NiceMock<MockContentControllerDelayed>(); }

  void SetUp() override {
    gfxPlatform::GetPlatform();
    // This pref is changed in Pan() without using SCOPED_GFX_PREF
    // because the modified value needs to be in place until the touch
    // events are processed, which may not happen until the input queue
    // is flushed in TearDown(). So, we save and restore its value here.
    mTouchStartTolerance = StaticPrefs::apz_touch_start_tolerance();
  }

  void TearDown() override {
    Preferences::SetFloat("apz.touch_start_tolerance", mTouchStartTolerance);
  }

  enum class PanOptions {
    None = 0,
    KeepFingerDown = 0x1,
    /*
     * Do not adjust the touch-start coordinates to overcome the touch-start
     * tolerance threshold. If this option is passed, it's up to the caller
     * to pass in coordinates that are sufficient to overcome the touch-start
     * tolerance *and* cause the desired amount of scrolling.
     */
    ExactCoordinates = 0x2,
    NoFling = 0x4
  };

  enum class PinchFlags {
    None = 0,
    LiftFinger1 = 0x1,
    LiftFinger2 = 0x2,
    /*
     * The bitwise OR result of (LiftFinger1 | LiftFinger2).
     * Defined explicitly here because it is used as the default
     * argument for PinchWithTouchInput which is defined BEFORE the
     * definition of operator| for this class.
     */
    LiftBothFingers = 0x3
  };

  template <class InputReceiver>
  APZEventResult Tap(const RefPtr<InputReceiver>& aTarget,
                     const ScreenIntPoint& aPoint, TimeDuration aTapLength,
                     nsEventStatus (*aOutEventStatuses)[2] = nullptr);

  template <class InputReceiver>
  void TapAndCheckStatus(const RefPtr<InputReceiver>& aTarget,
                         const ScreenIntPoint& aPoint, TimeDuration aTapLength);

  template <class InputReceiver>
  void Pan(const RefPtr<InputReceiver>& aTarget,
           const ScreenIntPoint& aTouchStart, const ScreenIntPoint& aTouchEnd,
           PanOptions aOptions = PanOptions::None,
           nsTArray<uint32_t>* aAllowedTouchBehaviors = nullptr,
           nsEventStatus (*aOutEventStatuses)[4] = nullptr,
           uint64_t* aOutInputBlockId = nullptr);

  /*
   * A version of Pan() that only takes y coordinates rather than (x, y) points
   * for the touch start and end points, and uses 10 for the x coordinates.
   * This is for convenience, as most tests only need to pan in one direction.
   */
  template <class InputReceiver>
  void Pan(const RefPtr<InputReceiver>& aTarget, int aTouchStartY,
           int aTouchEndY, PanOptions aOptions = PanOptions::None,
           nsTArray<uint32_t>* aAllowedTouchBehaviors = nullptr,
           nsEventStatus (*aOutEventStatuses)[4] = nullptr,
           uint64_t* aOutInputBlockId = nullptr);

  /*
   * Dispatches mock touch events to the apzc and checks whether apzc properly
   * consumed them and triggered scrolling behavior.
   */
  template <class InputReceiver>
  void PanAndCheckStatus(const RefPtr<InputReceiver>& aTarget, int aTouchStartY,
                         int aTouchEndY, bool aExpectConsumed,
                         nsTArray<uint32_t>* aAllowedTouchBehaviors,
                         uint64_t* aOutInputBlockId = nullptr);

  template <class InputReceiver>
  void DoubleTap(const RefPtr<InputReceiver>& aTarget,
                 const ScreenIntPoint& aPoint,
                 nsEventStatus (*aOutEventStatuses)[4] = nullptr,
                 uint64_t (*aOutInputBlockIds)[2] = nullptr);

  template <class InputReceiver>
  void DoubleTapAndCheckStatus(const RefPtr<InputReceiver>& aTarget,
                               const ScreenIntPoint& aPoint,
                               uint64_t (*aOutInputBlockIds)[2] = nullptr);

  struct PinchOptions {
    nsTArray<uint32_t>* mAllowedTouchBehaviors = nullptr;
    nsEventStatus (*mOutEventStatuses)[4] = nullptr;
    uint64_t* mOutInputBlockId = nullptr;
    PinchFlags mFlags = PinchFlags::LiftBothFingers;
    bool mVertical = false;
    int* mInputId = nullptr;
    Maybe<ScreenIntPoint> mSecondFocus;
    TimeDuration mTimeBetweenTouchEvents = TimeDuration::FromMilliseconds(20);

    // Workaround for https://github.com/llvm/llvm-project/issues/36032
    PinchOptions() {}

    // Fluent interface
    PinchOptions& AllowedTouchBehaviors(
        nsTArray<uint32_t>* aAllowedTouchBehaviors) {
      mAllowedTouchBehaviors = aAllowedTouchBehaviors;
      return *this;
    }
    PinchOptions& OutEventStatuses(nsEventStatus (*aOutEventStatuses)[4]) {
      mOutEventStatuses = aOutEventStatuses;
      return *this;
    }
    PinchOptions& OutInputBlockId(uint64_t* aOutInputBlockId) {
      mOutInputBlockId = aOutInputBlockId;
      return *this;
    }
    PinchOptions& Flags(PinchFlags aFlags) {
      mFlags = aFlags;
      return *this;
    }
    PinchOptions& Vertical(bool aVertical) {
      mVertical = aVertical;
      return *this;
    }
    PinchOptions& InputId(int& aInputId) {
      mInputId = &aInputId;
      return *this;
    }
    PinchOptions& SecondFocus(const ScreenIntPoint& aSecondFocus) {
      mSecondFocus = Some(aSecondFocus);
      return *this;
    }
    PinchOptions& TimeBetweenTouchEvents(const TimeDuration& aDuration) {
      mTimeBetweenTouchEvents = aDuration;
      return *this;
    }
  };

  // Pinch with one focus point. Zooms in place with no panning
  template <class InputReceiver>
  void PinchWithTouchInput(const RefPtr<InputReceiver>& aTarget,
                           const ScreenIntPoint& aFocus, float aScale,
                           PinchOptions aOptions = PinchOptions());

  template <class InputReceiver>
  void PinchWithTouchInputAndCheckStatus(
      const RefPtr<InputReceiver>& aTarget, const ScreenIntPoint& aFocus,
      float aScale, int& inputId, bool aShouldTriggerPinch,
      nsTArray<uint32_t>* aAllowedTouchBehaviors);

  template <class InputReceiver>
  void PinchWithPinchInput(const RefPtr<InputReceiver>& aTarget,
                           const ScreenIntPoint& aFocus,
                           const ScreenIntPoint& aSecondFocus, float aScale,
                           nsEventStatus (*aOutEventStatuses)[3] = nullptr);

  template <class InputReceiver>
  void PinchWithPinchInputAndCheckStatus(const RefPtr<InputReceiver>& aTarget,
                                         const ScreenIntPoint& aFocus,
                                         float aScale,
                                         bool aShouldTriggerPinch);

 protected:
  RefPtr<MockContentControllerDelayed> mcc;

 private:
  float mTouchStartTolerance;
};

MOZ_MAKE_ENUM_CLASS_BITWISE_OPERATORS(APZCTesterBase::PanOptions)
MOZ_MAKE_ENUM_CLASS_BITWISE_OPERATORS(APZCTesterBase::PinchFlags)

template <class InputReceiver>
APZEventResult APZCTesterBase::Tap(const RefPtr<InputReceiver>& aTarget,
                                   const ScreenIntPoint& aPoint,
                                   TimeDuration aTapLength,
                                   nsEventStatus (*aOutEventStatuses)[2]) {
  APZEventResult touchDownResult = TouchDown(aTarget, aPoint, mcc->Time());
  if (aOutEventStatuses) {
    (*aOutEventStatuses)[0] = touchDownResult.GetStatus();
  }
  mcc->AdvanceBy(aTapLength);

  // If touch-action is enabled then simulate the allowed touch behaviour
  // notification that the main thread is supposed to deliver.
  if (touchDownResult.GetStatus() != nsEventStatus_eConsumeNoDefault) {
    SetDefaultAllowedTouchBehavior(aTarget, touchDownResult.mInputBlockId);
  }

  APZEventResult touchUpResult = TouchUp(aTarget, aPoint, mcc->Time());
  if (aOutEventStatuses) {
    (*aOutEventStatuses)[1] = touchUpResult.GetStatus();
  }
  return touchDownResult;
}

template <class InputReceiver>
void APZCTesterBase::TapAndCheckStatus(const RefPtr<InputReceiver>& aTarget,
                                       const ScreenIntPoint& aPoint,
                                       TimeDuration aTapLength) {
  nsEventStatus statuses[2];
  Tap(aTarget, aPoint, aTapLength, &statuses);
  EXPECT_EQ(nsEventStatus_eConsumeDoDefault, statuses[0]);
  EXPECT_EQ(nsEventStatus_eConsumeDoDefault, statuses[1]);
}

template <class InputReceiver>
void APZCTesterBase::Pan(const RefPtr<InputReceiver>& aTarget,
                         const ScreenIntPoint& aTouchStart,
                         const ScreenIntPoint& aTouchEnd, PanOptions aOptions,
                         nsTArray<uint32_t>* aAllowedTouchBehaviors,
                         nsEventStatus (*aOutEventStatuses)[4],
                         uint64_t* aOutInputBlockId) {
  // Reduce the move tolerance to a tiny value.
  // We can't use a scoped pref because this value might be read at some later
  // time when the events are actually processed, rather than when we deliver
  // them.
  const float touchStartTolerance = 0.1f;
  const float panThreshold = touchStartTolerance * aTarget->GetDPI();
  Preferences::SetFloat("apz.touch_start_tolerance", touchStartTolerance);
  Preferences::SetFloat("apz.touch_move_tolerance", 0.0f);
  int overcomeTouchToleranceX = 0;
  int overcomeTouchToleranceY = 0;
  if (!(aOptions & PanOptions::ExactCoordinates)) {
    // Have the direction of the adjustment to overcome the touch tolerance
    // match the direction of the entire gesture, otherwise we run into
    // trouble such as accidentally activating the axis lock.
    if (aTouchStart.x != aTouchEnd.x && aTouchStart.y != aTouchEnd.y) {
      // Tests that need to avoid rounding error here can arrange for
      // panThreshold to be 10 (by setting the DPI to 100), which makes sure
      // that these are the legs in a Pythagorean triple where panThreshold is
      // the hypotenuse. Watch out for changes of APZCPinchTester::mDPI.
      overcomeTouchToleranceX = panThreshold / 10 * 6;
      overcomeTouchToleranceY = panThreshold / 10 * 8;
    } else if (aTouchStart.x != aTouchEnd.x) {
      overcomeTouchToleranceX = panThreshold;
    } else if (aTouchStart.y != aTouchEnd.y) {
      overcomeTouchToleranceY = panThreshold;
    }
  }

  const TimeDuration TIME_BETWEEN_TOUCH_EVENT =
      TimeDuration::FromMilliseconds(20);

  // Even if the caller doesn't care about the block id, we need it to set the
  // allowed touch behaviour below, so make sure aOutInputBlockId is non-null.
  uint64_t blockId;
  if (!aOutInputBlockId) {
    aOutInputBlockId = &blockId;
  }

  // Make sure the move is large enough to not be handled as a tap
  APZEventResult result =
      TouchDown(aTarget,
                ScreenIntPoint(aTouchStart.x + overcomeTouchToleranceX,
                               aTouchStart.y + overcomeTouchToleranceY),
                mcc->Time());
  if (aOutInputBlockId) {
    *aOutInputBlockId = result.mInputBlockId;
  }
  if (aOutEventStatuses) {
    (*aOutEventStatuses)[0] = result.GetStatus();
  }

  mcc->AdvanceBy(TIME_BETWEEN_TOUCH_EVENT);

  // Allowed touch behaviours must be set after sending touch-start.
  if (result.GetStatus() != nsEventStatus_eConsumeNoDefault) {
    if (aAllowedTouchBehaviors) {
      EXPECT_EQ(1UL, aAllowedTouchBehaviors->Length());
      aTarget->SetAllowedTouchBehavior(*aOutInputBlockId,
                                       *aAllowedTouchBehaviors);
    } else {
      SetDefaultAllowedTouchBehavior(aTarget, *aOutInputBlockId);
    }
  }

  result = TouchMove(aTarget, aTouchStart, mcc->Time());
  if (aOutEventStatuses) {
    (*aOutEventStatuses)[1] = result.GetStatus();
  }

  mcc->AdvanceBy(TIME_BETWEEN_TOUCH_EVENT);

  const int numSteps = 3;
  auto stepVector = (aTouchEnd - aTouchStart) / numSteps;
  for (int k = 1; k < numSteps; k++) {
    auto stepPoint = aTouchStart + stepVector * k;
    Unused << TouchMove(aTarget, stepPoint, mcc->Time());

    mcc->AdvanceBy(TIME_BETWEEN_TOUCH_EVENT);
  }

  result = TouchMove(aTarget, aTouchEnd, mcc->Time());
  if (aOutEventStatuses) {
    (*aOutEventStatuses)[2] = result.GetStatus();
  }

  mcc->AdvanceBy(TIME_BETWEEN_TOUCH_EVENT);

  if (!(aOptions & PanOptions::KeepFingerDown)) {
    result = TouchUp(aTarget, aTouchEnd, mcc->Time());
  } else {
    result.SetStatusAsIgnore();
  }
  if (aOutEventStatuses) {
    (*aOutEventStatuses)[3] = result.GetStatus();
  }

  if ((aOptions & PanOptions::NoFling)) {
    aTarget->CancelAnimation();
  }

  // Don't increment the time here. Animations started on touch-up, such as
  // flings, are affected by elapsed time, and we want to be able to sample
  // them immediately after they start, without time having elapsed.
}

template <class InputReceiver>
void APZCTesterBase::Pan(const RefPtr<InputReceiver>& aTarget, int aTouchStartY,
                         int aTouchEndY, PanOptions aOptions,
                         nsTArray<uint32_t>* aAllowedTouchBehaviors,
                         nsEventStatus (*aOutEventStatuses)[4],
                         uint64_t* aOutInputBlockId) {
  Pan(aTarget, ScreenIntPoint(10, aTouchStartY), ScreenIntPoint(10, aTouchEndY),
      aOptions, aAllowedTouchBehaviors, aOutEventStatuses, aOutInputBlockId);
}

template <class InputReceiver>
void APZCTesterBase::PanAndCheckStatus(
    const RefPtr<InputReceiver>& aTarget, int aTouchStartY, int aTouchEndY,
    bool aExpectConsumed, nsTArray<uint32_t>* aAllowedTouchBehaviors,
    uint64_t* aOutInputBlockId) {
  nsEventStatus statuses[4];  // down, move, move, up
  Pan(aTarget, aTouchStartY, aTouchEndY, PanOptions::None,
      aAllowedTouchBehaviors, &statuses, aOutInputBlockId);

  EXPECT_EQ(nsEventStatus_eConsumeDoDefault, statuses[0]);

  nsEventStatus touchMoveStatus;
  if (aExpectConsumed) {
    touchMoveStatus = nsEventStatus_eConsumeDoDefault;
  } else {
    touchMoveStatus = nsEventStatus_eIgnore;
  }
  EXPECT_EQ(touchMoveStatus, statuses[1]);
  EXPECT_EQ(touchMoveStatus, statuses[2]);
}

template <class InputReceiver>
void APZCTesterBase::DoubleTap(const RefPtr<InputReceiver>& aTarget,
                               const ScreenIntPoint& aPoint,
                               nsEventStatus (*aOutEventStatuses)[4],
                               uint64_t (*aOutInputBlockIds)[2]) {
  APZEventResult result = TouchDown(aTarget, aPoint, mcc->Time());
  if (aOutEventStatuses) {
    (*aOutEventStatuses)[0] = result.GetStatus();
  }
  if (aOutInputBlockIds) {
    (*aOutInputBlockIds)[0] = result.mInputBlockId;
  }
  mcc->AdvanceByMillis(10);

  // If touch-action is enabled then simulate the allowed touch behaviour
  // notification that the main thread is supposed to deliver.
  if (result.GetStatus() != nsEventStatus_eConsumeNoDefault) {
    SetDefaultAllowedTouchBehavior(aTarget, result.mInputBlockId);
  }

  result = TouchUp(aTarget, aPoint, mcc->Time());
  if (aOutEventStatuses) {
    (*aOutEventStatuses)[1] = result.GetStatus();
  }
  mcc->AdvanceByMillis(10);
  result = TouchDown(aTarget, aPoint, mcc->Time());
  if (aOutEventStatuses) {
    (*aOutEventStatuses)[2] = result.GetStatus();
  }
  if (aOutInputBlockIds) {
    (*aOutInputBlockIds)[1] = result.mInputBlockId;
  }
  mcc->AdvanceByMillis(10);

  if (result.GetStatus() != nsEventStatus_eConsumeNoDefault) {
    SetDefaultAllowedTouchBehavior(aTarget, result.mInputBlockId);
  }

  result = TouchUp(aTarget, aPoint, mcc->Time());
  if (aOutEventStatuses) {
    (*aOutEventStatuses)[3] = result.GetStatus();
  }
}

template <class InputReceiver>
void APZCTesterBase::DoubleTapAndCheckStatus(
    const RefPtr<InputReceiver>& aTarget, const ScreenIntPoint& aPoint,
    uint64_t (*aOutInputBlockIds)[2]) {
  nsEventStatus statuses[4];
  DoubleTap(aTarget, aPoint, &statuses, aOutInputBlockIds);
  EXPECT_EQ(nsEventStatus_eConsumeDoDefault, statuses[0]);
  EXPECT_EQ(nsEventStatus_eConsumeDoDefault, statuses[1]);
  EXPECT_EQ(nsEventStatus_eConsumeDoDefault, statuses[2]);
  EXPECT_EQ(nsEventStatus_eConsumeDoDefault, statuses[3]);
}

template <class InputReceiver>
void APZCTesterBase::PinchWithTouchInput(const RefPtr<InputReceiver>& aTarget,
                                         const ScreenIntPoint& aFocus,
                                         float aScale, PinchOptions aOptions) {
  // Having pinch coordinates in float type may cause problems with
  // high-precision scale values since SingleTouchData accepts integer value.
  // But for trivial tests it should be ok.
  const float pinchLength = 100.0;
  const float pinchLengthScaled = pinchLength * aScale;

  const float pinchLengthX = aOptions.mVertical ? 0 : pinchLength;
  const float pinchLengthScaledX = aOptions.mVertical ? 0 : pinchLengthScaled;
  const float pinchLengthY = aOptions.mVertical ? pinchLength : 0;
  const float pinchLengthScaledY = aOptions.mVertical ? pinchLengthScaled : 0;

  // Even if the caller doesn't care about the block id, we need it to set the
  // allowed touch behaviour below, so make sure aOutInputBlockId is non-null.
  uint64_t blockId;
  if (!aOptions.mOutInputBlockId) {
    aOptions.mOutInputBlockId = &blockId;
  }

  int inputId = aOptions.mInputId ? *aOptions.mInputId : 0;

  // If a second focus point is not specified in the pinch options, use the
  // same focus point throughout the gesture.
  ScreenIntPoint secondFocus =
      aOptions.mSecondFocus.isSome() ? *aOptions.mSecondFocus : aFocus;

  MultiTouchInput mtiStart =
      MultiTouchInput(MultiTouchInput::MULTITOUCH_START, 0, mcc->Time(), 0);
  mtiStart.mTouches.AppendElement(CreateSingleTouchData(inputId, aFocus));
  mtiStart.mTouches.AppendElement(CreateSingleTouchData(inputId + 1, aFocus));
  APZEventResult result;
  result = aTarget->ReceiveInputEvent(mtiStart);
  if (aOptions.mOutInputBlockId) {
    *aOptions.mOutInputBlockId = result.mInputBlockId;
  }
  if (aOptions.mOutEventStatuses) {
    (*aOptions.mOutEventStatuses)[0] = result.GetStatus();
  }

  if (aOptions.mAllowedTouchBehaviors) {
    EXPECT_EQ(2UL, aOptions.mAllowedTouchBehaviors->Length());
    aTarget->SetAllowedTouchBehavior(*aOptions.mOutInputBlockId,
                                     *aOptions.mAllowedTouchBehaviors);
  } else {
    SetDefaultAllowedTouchBehavior(aTarget, *aOptions.mOutInputBlockId, 2);
  }

  mcc->AdvanceBy(aOptions.mTimeBetweenTouchEvents);

  ScreenIntPoint pinchStartPoint1(aFocus.x - int32_t(pinchLengthX),
                                  aFocus.y - int32_t(pinchLengthY));
  ScreenIntPoint pinchStartPoint2(aFocus.x + int32_t(pinchLengthX),
                                  aFocus.y + int32_t(pinchLengthY));

  MultiTouchInput mtiMove1 =
      MultiTouchInput(MultiTouchInput::MULTITOUCH_MOVE, 0, mcc->Time(), 0);
  mtiMove1.mTouches.AppendElement(
      CreateSingleTouchData(inputId, pinchStartPoint1));
  mtiMove1.mTouches.AppendElement(
      CreateSingleTouchData(inputId + 1, pinchStartPoint2));
  result = aTarget->ReceiveInputEvent(mtiMove1);
  if (aOptions.mOutEventStatuses) {
    (*aOptions.mOutEventStatuses)[1] = result.GetStatus();
  }

  mcc->AdvanceBy(aOptions.mTimeBetweenTouchEvents);

  // Pinch instantly but move in steps.
  const int numSteps = 3;
  auto stepVector = (secondFocus - aFocus) / numSteps;
  for (int k = 1; k < numSteps; k++) {
    ScreenIntPoint stepFocus = aFocus + stepVector * k;
    ScreenIntPoint stepPoint1(stepFocus.x - int32_t(pinchLengthScaledX),
                              stepFocus.y - int32_t(pinchLengthScaledY));
    ScreenIntPoint stepPoint2(stepFocus.x + int32_t(pinchLengthScaledX),
                              stepFocus.y + int32_t(pinchLengthScaledY));
    MultiTouchInput mtiMoveStep =
        MultiTouchInput(MultiTouchInput::MULTITOUCH_MOVE, 0, mcc->Time(), 0);
    mtiMoveStep.mTouches.AppendElement(
        CreateSingleTouchData(inputId, stepPoint1));
    mtiMoveStep.mTouches.AppendElement(
        CreateSingleTouchData(inputId + 1, stepPoint2));
    Unused << aTarget->ReceiveInputEvent(mtiMoveStep);

    mcc->AdvanceBy(aOptions.mTimeBetweenTouchEvents);
  }

  ScreenIntPoint pinchEndPoint1(secondFocus.x - int32_t(pinchLengthScaledX),
                                secondFocus.y - int32_t(pinchLengthScaledY));
  ScreenIntPoint pinchEndPoint2(secondFocus.x + int32_t(pinchLengthScaledX),
                                secondFocus.y + int32_t(pinchLengthScaledY));

  MultiTouchInput mtiMove2 =
      MultiTouchInput(MultiTouchInput::MULTITOUCH_MOVE, 0, mcc->Time(), 0);
  mtiMove2.mTouches.AppendElement(
      CreateSingleTouchData(inputId, pinchEndPoint1));
  mtiMove2.mTouches.AppendElement(
      CreateSingleTouchData(inputId + 1, pinchEndPoint2));
  result = aTarget->ReceiveInputEvent(mtiMove2);
  if (aOptions.mOutEventStatuses) {
    (*aOptions.mOutEventStatuses)[2] = result.GetStatus();
  }

  if (aOptions.mFlags & (PinchFlags::LiftFinger1 | PinchFlags::LiftFinger2)) {
    mcc->AdvanceBy(aOptions.mTimeBetweenTouchEvents);

    MultiTouchInput mtiEnd =
        MultiTouchInput(MultiTouchInput::MULTITOUCH_END, 0, mcc->Time(), 0);
    if (aOptions.mFlags & PinchFlags::LiftFinger1) {
      mtiEnd.mTouches.AppendElement(
          CreateSingleTouchData(inputId, pinchEndPoint1));
    }
    if (aOptions.mFlags & PinchFlags::LiftFinger2) {
      mtiEnd.mTouches.AppendElement(
          CreateSingleTouchData(inputId + 1, pinchEndPoint2));
    }
    result = aTarget->ReceiveInputEvent(mtiEnd);
    if (aOptions.mOutEventStatuses) {
      (*aOptions.mOutEventStatuses)[3] = result.GetStatus();
    }
  }

  inputId += 2;

  if (aOptions.mInputId) {
    *aOptions.mInputId = inputId;
  }
}

template <class InputReceiver>
void APZCTesterBase::PinchWithTouchInputAndCheckStatus(
    const RefPtr<InputReceiver>& aTarget, const ScreenIntPoint& aFocus,
    float aScale, int& inputId, bool aShouldTriggerPinch,
    nsTArray<uint32_t>* aAllowedTouchBehaviors) {
  nsEventStatus statuses[4];  // down, move, move, up
  PinchWithTouchInput(aTarget, aFocus, aScale,
                      PinchOptions()
                          .AllowedTouchBehaviors(aAllowedTouchBehaviors)
                          .OutEventStatuses(&statuses)
                          .InputId(inputId));

  nsEventStatus expectedMoveStatus = aShouldTriggerPinch
                                         ? nsEventStatus_eConsumeDoDefault
                                         : nsEventStatus_eIgnore;
  EXPECT_EQ(nsEventStatus_eConsumeDoDefault, statuses[0]);
  EXPECT_EQ(expectedMoveStatus, statuses[1]);
  EXPECT_EQ(expectedMoveStatus, statuses[2]);
}

template <class InputReceiver>
void APZCTesterBase::PinchWithPinchInput(
    const RefPtr<InputReceiver>& aTarget, const ScreenIntPoint& aFocus,
    const ScreenIntPoint& aSecondFocus, float aScale,
    nsEventStatus (*aOutEventStatuses)[3]) {
  const TimeDuration TIME_BETWEEN_PINCH_INPUT =
      TimeDuration::FromMilliseconds(50);

  auto event = CreatePinchGestureInput(PinchGestureInput::PINCHGESTURE_START,
                                       aFocus, 10.0, 10.0, mcc->Time());
  APZEventResult actual = aTarget->ReceiveInputEvent(event);
  if (aOutEventStatuses) {
    (*aOutEventStatuses)[0] = actual.GetStatus();
  }
  mcc->AdvanceBy(TIME_BETWEEN_PINCH_INPUT);

  event =
      CreatePinchGestureInput(PinchGestureInput::PINCHGESTURE_SCALE,
                              aSecondFocus, 10.0 * aScale, 10.0, mcc->Time());
  actual = aTarget->ReceiveInputEvent(event);
  if (aOutEventStatuses) {
    (*aOutEventStatuses)[1] = actual.GetStatus();
  }
  mcc->AdvanceBy(TIME_BETWEEN_PINCH_INPUT);

  event =
      CreatePinchGestureInput(PinchGestureInput::PINCHGESTURE_END, aSecondFocus,
                              10.0 * aScale, 10.0 * aScale, mcc->Time());
  actual = aTarget->ReceiveInputEvent(event);
  if (aOutEventStatuses) {
    (*aOutEventStatuses)[2] = actual.GetStatus();
  }
}

template <class InputReceiver>
void APZCTesterBase::PinchWithPinchInputAndCheckStatus(
    const RefPtr<InputReceiver>& aTarget, const ScreenIntPoint& aFocus,
    float aScale, bool aShouldTriggerPinch) {
  nsEventStatus statuses[3];  // scalebegin, scale, scaleend
  PinchWithPinchInput(aTarget, aFocus, aFocus, aScale, &statuses);

  nsEventStatus expectedStatus = aShouldTriggerPinch
                                     ? nsEventStatus_eConsumeDoDefault
                                     : nsEventStatus_eIgnore;
  EXPECT_EQ(expectedStatus, statuses[0]);
  EXPECT_EQ(expectedStatus, statuses[1]);
}

inline FrameMetrics TestFrameMetrics() {
  FrameMetrics fm;

  fm.SetDisplayPort(CSSRect(0, 0, 10, 10));
  fm.SetCompositionBounds(ParentLayerRect(0, 0, 10, 10));
  fm.SetScrollableRect(CSSRect(0, 0, 100, 100));

  return fm;
}

#endif  // mozilla_layers_APZTestCommon_h
