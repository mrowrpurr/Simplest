#pragma once

#include <Simp1e/ComponentCast.h>
#include <Simp1e/IImageComponent.h>
#include <Simp1e/ILineColorComponent.h>
#include <Simp1e/IPositionComponent.h>
#include <Simp1e/IRotationComponent.h>
#include <Simp1e/ISizeComponent.h>
#include <Simp1e/Rectangle.h>
#include <Simp1e/ToQColor.h>
#include <Simp1e/ToQPoint.h>
#include <Simp1e/ToQRectF.h>
#include <_Log_.h>

#include "IQtComponentPainter.h"
#include "QtSimp1eImageComponent.h"

namespace Simp1e {

    // NOTE: for Svg, the Svg item needs to be added to the scene and we don't directly paint it!

    class QtImageComponentPainter : public IQtComponentPainter {
    public:
        void Paint(
            IEngine* engine, Entity entity, void* component, QPainter* painter, const QStyleOptionGraphicsItem* option,
            QWidget* widget
        ) override {
            auto* imageComponent = component_cast<IImageComponent>(component);
            if (!imageComponent) return;  // TODO fix it so Painters() only get called for the right components

            _Log_("Paint image for entity {}", entity);

            auto* entityManager = engine->GetEntities();

            // _Log_("PAINT IMAGE - PAINT IMAGE - PAINT IMAGE");

            auto* sizeComponent = entityManager->GetComponent<ISizeComponent>(entity);
            if (!sizeComponent) {
                _Log_("[Image Painter] No size component found for entity {}", entity);
                return;
            }

            auto* positionComponent = entityManager->GetComponent<IPositionComponent>(entity);
            if (!positionComponent) {
                _Log_("[Image Painter] No position component found for entity {}", entity);
                return;
            }

            auto* qImageComponent = entityManager->GetComponent<QtSimp1eImageComponent>(entity);
            if (!qImageComponent) {
                _Log_("[Image Painter] No image component found for entity {}", entity);
                return;
            }

            auto* qImage = qImageComponent->GetQSimp1eImage();

            if (qImage->GetImageRenderType() == ImageRenderType::Vector) {
                _Log_("SVG not supported yet");
                return;
            }

            qImage->SetSize(sizeComponent->GetSize());

            // auto* rotationComponent = entityManager->GetComponent<IRotationComponent>(entity);
            // if (rotationComponent) {
            //     _Log_("---------> Image Painter - Rotate to {}", rotationComponent->GetRotation());
            //     qImage->RotateTo(rotationComponent->GetRotation());
            // } else {
            //     _Log_("---------> Entity {} has no rotation component", entity);
            // }

            auto* pixmap = qImage->GetPixmap();
            if (!pixmap) {
                _Log_("[Image Painter] No pixmap found for entity {}", entity);
                return;
            }

            _Log_(
                "Painting image {} at {},{}", imageComponent->GetImagePath(), positionComponent->GetPosition().x(),
                positionComponent->GetPosition().y()
            );
            painter->drawPixmap(QPointF{0, 0}, *pixmap);
            // painter->drawPixmap(ToQPoint(positionComponent->GetPosition().ToPoint()), *pixmap);

            auto* lineColor = entityManager->GetComponent<ILineColorComponent>(entity);
            if (lineColor) {
                painter->setPen(QPen(ToQColor(lineColor->GetColor()), 5.0, Qt::SolidLine));
                painter->drawRect(ToQRectF(Rectangle({0, 0}, qImage->GetActualSize())));
            }
        }
    };
}
