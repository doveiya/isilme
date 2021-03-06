/*      _______   __   __   __   ______   __   __   _______   __   __
 *     / _____/\ / /\ / /\ / /\ / ____/\ / /\ / /\ / ___  /\ /  |\/ /\
 *    / /\____\// / // / // / // /\___\// /_// / // /\_/ / // , |/ / /
 *   / / /__   / / // / // / // / /    / ___  / // ___  / // /| ' / /
 *  / /_// /\ / /_// / // / // /_/_   / / // / // /\_/ / // / |  / /
 * /______/ //______/ //_/ //_____/\ /_/ //_/ //_/ //_/ //_/ /|_/ /
 * \______\/ \______\/ \_\/ \_____\/ \_\/ \_\/ \_\/ \_\/ \_\/ \_\/
 *
 * Copyright (c) 2004 - 2008 Olof Naess�n and Per Larsson
 *
 *
 * Per Larsson a.k.a finalman
 * Olof Naess�n a.k.a jansem/yakslem
 *
 * Visit: http://guichan.sourceforge.net
 *
 * License: (BSD)
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name of Guichan nor the names of its contributors may
 *    be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * For comments regarding functions please see the header file.
 */

#include "guichan/widgets/tab.hpp"

#include "guichan/font.hpp"
#include "guichan/graphics.hpp"
#include "guichan/widgets/button.hpp"
#include "guichan/widgets/label.hpp"
#include "guichan/widgets/tabbedarea.hpp"

namespace gcn
{
    Tab::Tab()
            :mHasMouse(false)
    {
        addMouseListener(this);
    }

    Tab::~Tab()
    {
    }

    void Tab::AdjustSize()
    {
        SetSize(mLabel->GetWidth() + 8,
                mLabel->GetHeight() + 8);

        if (mTabbedArea.lock())
        {
            mTabbedArea.lock()->adjustTabPositions();
        }
    }

    void Tab::SetTabbedArea(TabbedAreaPtr tabbedArea)
    {
        mTabbedArea = tabbedArea;
    }

    TabbedAreaPtr Tab::GetTabbedArea()
    {
        return mTabbedArea.lock();
    }

    void Tab::SetCaption(const std::string& caption)
    {
        mLabel->setCaption(caption);
        mLabel->adjustSize();
        AdjustSize();
    }

    const std::string& Tab::GetCaption() const
    {
        return mLabel->getCaption();
    }

    void Tab::Draw(GraphicsPtr graphics)
    {
        const Color &faceColor = getBaseColor();
        const int alpha = getBaseColor().a;
        Color highlightColor = faceColor + 0x303030;
        highlightColor.a = alpha;
        Color shadowColor = faceColor - 0x303030;
        shadowColor.a = alpha;

        Color borderColor;
        Color baseColor;

        if ((mTabbedArea.lock() && mTabbedArea.lock()->IsTabSelected(boost::shared_dynamic_cast<Tab>(shared_from_this())))
            || mHasMouse)
        {
            // Draw a border.
            graphics->setColor(highlightColor);
            graphics->drawLine(0, 0, GetWidth() - 1, 0);
            graphics->drawLine(0, 1, 0, GetHeight() - 1);
            graphics->setColor(shadowColor);
            graphics->drawLine(GetWidth() - 1, 1, GetWidth() - 1, GetHeight() - 1);

            borderColor = highlightColor;
            baseColor = getBaseColor();
        }
        else
        {
            // Draw a border.
            graphics->setColor(shadowColor);
            graphics->drawLine(0, 0, GetWidth() - 1, 0);
            graphics->drawLine(0, 1, 0, GetHeight() - 1);
            graphics->drawLine(GetWidth() - 1, 1, GetWidth() - 1, GetHeight() - 1);

            baseColor = getBaseColor() - 0x151515;
            baseColor.a = alpha;
        }

        // Push a clip area so the other drawings don't need to worry
        // about the border.
        graphics->pushClipArea(Rectangle(1, 1, GetWidth() - 2, GetHeight() - 1));
        const Rectangle currentClipArea = graphics->getCurrentClipArea();

        graphics->setColor(baseColor);
        graphics->fillRectangle(Rectangle(0, 
                                          0, 
                                          currentClipArea.width, 
                                          currentClipArea.height));

        DrawChildren(graphics);

        if (mTabbedArea.lock()
            && mTabbedArea.lock()->IsFocused()
            && mTabbedArea.lock()->IsTabSelected(boost::shared_dynamic_cast<Tab>(shared_from_this())))
        {
            graphics->setColor(Color(0x000000));
            graphics->drawRectangle(Rectangle(2,
                                              2,
                                              currentClipArea.width - 4,
                                              currentClipArea.height - 4));
        }

        graphics->popClipArea();
    }

    void Tab::MouseEntered(MouseEvent& mouseEvent)
    {
        mHasMouse = true;
    }

    void Tab::MouseExited(MouseEvent& mouseEvent)
    {
        mHasMouse = false;
    }

	void Tab::Initialise()
	{
		mLabel = LabelPtr(new Label());
		mLabel->SetPosition(4, 4);
		Add(mLabel);
	}

}

