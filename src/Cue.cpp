#include "Cue.hpp"

#pragma comment(lib, "gdiplus.lib")

using namespace Gdiplus;

Cue::Cue() :
	textureSize_(),
	mouse_(),
	ball_(),
	cue_(),
	auxiliaryLine_()
{
	textureSize_.width  = 1100 / 5; // Çàãíàòü â êîíñòàíòû
	textureSize_.height = 360  / 5; //

	textureSize_ *= static_cast<float>(SCALE);
}

Cue::~Cue()
{
	//delete(texture_);
}

VOID Cue::draw(Graphics *graphics, Pen *pen, Image *image)
{	
	cue_.draw((ball_ - cue_).getX(), (ball_ - cue_).getY(), graphics, pen);
	auxiliaryLine_.draw((ball_ - auxiliaryLine_).getX(), (ball_ - auxiliaryLine_).getY(), graphics, pen, Color::Gray);

	pen->SetColor(Color::Gray);
	pen->SetWidth(3);
	graphics->DrawEllipse(pen, static_cast<INT>(mouse_.getX() - RShari / 2), static_cast<INT>(mouse_.getY() - RShari / 2), RShari, RShari);

	//graphics->ResetTransform();
    //graphics->TranslateTransform(ball_.getX(), ball_.getY());
    //graphics->RotateTransform((mouse_ - ball_).getK());

	//graphics->TranslateTransform(ball_.getX(), ball_.getY());
	//graphics->RotateTransform((mouse_ - ball_).getK());alf = atan_(mouse_.x-ball_.x,  mouse_.y -ball_.y);

	ImageAttributes imAttr;
	imAttr.SetColorKey(Color(0, 197, 206, 5), Color(0, 197, 206, 5));
	//graphics->DrawLine(pen, Point(static_cast<INT>(x0     ), static_cast<INT>(     y0)), Point(static_cast<INT>(x0 + x_), static_cast<INT>(y_ + y0)));//2-end
	graphics->DrawImage(image, Rect(static_cast<INT>(ball_.getX() - 884), static_cast<INT>(ball_.getY() - 20 / 2), 884, 20), 0, 0, 884, 20, Unit::UnitPixel, &imAttr, 0);

	graphics->ResetTransform();
}

VOID Cue::rotate(POINT point, LPARAM lParam)
{
	mouse_.setX(LOWORD(lParam));
	mouse_.setY(HIWORD(lParam));

	ball_.setX(point.x);
	ball_.setY(point.y);

	cue_ = mouse_ - ball_;
	auxiliaryLine_ = ball_ - mouse_;

	cue_.setL(CUE_LENGTH);
	cue_.setXY();
	auxiliaryLine_.setXY();
}