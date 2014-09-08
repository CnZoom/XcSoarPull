/*
Copyright_License {

  XCSoar Glide Computer - http://www.xcsoar.org/
  Copyright (C) 2000-2014 The XCSoar Project
  A detailed list of copyright holders can be found in the file "AUTHORS".

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
}
*/

#include "NextArrowLook.hpp"
#include "Screen/Layout.hpp"
#include "Asset.hpp"

void
NextArrowLook::Initialise(bool inverse)
{
  outline_pen.Set(Layout::Scale(1),
                  inverse
                ? (HasColors() ? LightColor(COLOR_GRAY) : COLOR_WHITE)
                : (HasColors() ? DarkColor(COLOR_GRAY) : COLOR_BLACK));
  brush[inverse ? BOTTOM : TOP].Set(IsDithered() ? COLOR_DARK_GRAY : DarkColor(COLOR_GRAY));
  brush[inverse ? TOP : BOTTOM].Set(IsDithered() ? COLOR_LIGHT_GRAY : LightColor(COLOR_GRAY));
}
