#pragma once

#include <Simp1e/EventCast.h>
#include <Simp1e/EventResult.h>
#include <Simp1e/Position.h>
#include <Simp1e/QtSimp1ePlatformDetection.h>
#include <_Log_.h>
#include <function_pointer.h>

#include <QApplication>
#include <QKeyEvent>
#include <memory>
#include <vector>

#ifdef SIMP1E_MOBILE
    #include <QAccelerometer>
    #include <QSensorReading>
#endif

namespace Simp1e {

    class QSimp1eApp : public QApplication {
        QEvent* _lastEvent;
        bool    _lastWasPress            = false;
        int     _lastKey                 = 0;
        int     _mockArgcForQApplication = 0;

        std::vector<std::unique_ptr<IFunctionPointer<EventResult::Value(QKeyEvent*)>>> _keyboardEventListeners;

#ifdef SIMP1E_MOBILE
        QAccelerometer _accelerometer;
#endif

    public:
        QSimp1eApp(int& argc, char** argv) : QApplication(argc, argv) { setStyle("Fusion"); }
        QSimp1eApp() : QSimp1eApp(_mockArgcForQApplication, nullptr) {}

        void OnKeyEvent(FunctionPointer<EventResult::Value(QKeyEvent*)> callback) {
            callback.do_not_destroy_function_pointer();
            _keyboardEventListeners.emplace_back(callback.inner_function_pointer());
        }

        // TODO: encapsulate into a Simp1e/IAccelerometer.h interface and Simp1e/QtAccelerometer.h implementation
        void StartAccelerometer() {
#ifdef SIMP1E_MOBILE
            _accelerometer.start();
#endif
        }

        void StopAccelerometer() {
#ifdef SIMP1E_MOBILE
            _accelerometer.stop();
#endif
        }

        PositionF GetAccelerometerReading() {
#ifdef SIMP1E_MOBILE
            auto reading = _accelerometer.reading();
            return {reading->x(), reading->y(), reading->z()};
#else
            return {};
#endif
        }

    protected:
        bool notify(QObject* receiver, QEvent* event) override {
            if (event->type() == QEvent::KeyPress || event->type() == QEvent::KeyRelease) {
                auto  isPress  = event->type() == QEvent::KeyPress;
                auto* keyEvent = static_cast<QKeyEvent*>(event);
                if (event == _lastEvent && isPress == _lastWasPress && keyEvent->key() == _lastKey) {
                    return QApplication::notify(receiver, event);
                } else {
                    _lastEvent    = event;
                    _lastWasPress = isPress;
                    _lastKey      = keyEvent->key();
                }
                for (auto& listener : _keyboardEventListeners)
                    if (listener->invoke(static_cast<QKeyEvent*>(event))) return true;
            }
            return QApplication::notify(receiver, event);
        }
    };
}
