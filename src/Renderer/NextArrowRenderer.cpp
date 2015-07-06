/*
Copyright_License {

  XCSoar Glide Computer - http://www.xcsoar.org/
  Copyright (C) 2000-2015 The XCSoar Project
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

#include "NextArrowRenderer.hpp"
#include "Look/WindArrowLook.hpp"
#include "Screen/Canvas.hpp"
#include "Screen/Layout.hpp"
#include "Math/Screen.hpp"
#include "NMEA/Derived.hpp"
#include "Util/Macros.hpp"


void
NextArrowRenderer::DrawArrow(Canvas &canvas, const PixelRect &rc,
                             Angle angle)
{
  /*
   * Define arrow geometry for forward pointing arrow.
   * These are the coordinates of the corners, relative to the center (o)
   *
   *                               +   (0,-head_len)
   *                              / \
   *                             /   \
   *                            /     \
   * (-head_width,-head_base)  +-+   +-+  (head_width,-head_base)
   *   (-tail_width,-head_base)  | o |  (tail_width,-head_base)
   *                             |   |
   *                             |   |
   *     (-tail_width,tail_len)  +---+  (tail_width,tail_len)
   *
   * The "tail" of the arrow is slightly shorter than the "head" to avoid
   * the corners of the tail to stick out of the bounding PixelRect.
   *
   * Notes about the scaling formula below:
   *  - size is the side of the largest square that fits in the rectangle
   *    rc. We want the arrow to fill a square of this size. (This is
   *    in the scale of real screen pixels.)
   *  - The arrow corner coordinates are essentially fractions of size, e.g.:
   *    head_width = 0.360 * size
   *  - To avoid floating point this is instead calculated as:
   *    head_width = 360 * size / 1000
   *  - DrawPolygon uses another coordinate system; it multiplies the
   *    coordinates by the integer factor returned by Layout::Scale(1).
   *    We divide by Layout::Scale(1) to counter this conversion:
   *    head_width = 360 * size / (Layout::Scale(1) * 1000)
   *
   *  - Note that Layout::Scale(1) * 1000 is not equivalent to
   *    Layout::Scale(1000). The latter includes a fraction in the
   *    scale that is not equal to the integer scaling factor used by
   *    DrawPolygon. The arrow will not fill the square if we use this.
   */
  const auto size = std::min(rc.right - rc.left, rc.bottom - rc.top);
  const auto scale = Layout::Scale(1) * 1000;
  const auto head_len = 500 * size / scale;
  const auto head_width = 360 * size / scale;
  const auto head_base = head_len - head_width;
  const auto tail_width = 160 * size / scale;
  const auto tail_len = head_len - tail_width / 2;

  // An array of the arrow corner coordinates.
  RasterPoint arrow[] = {
    { 0, -head_len },
    { head_width, -head_base },
    { tail_width, -head_base },
    { tail_width, tail_len },
    { -tail_width, tail_len },
    { -tail_width, -head_base },
    { -head_width, -head_base },
  };

  // Rotate the arrow, center it in the bounding rectangle
  PolygonRotateShift(arrow, ARRAY_SIZE(arrow), rc.GetCenter(), angle);

  // Draw the arrow.
  canvas.Select(look.arrow_pen);
  canvas.Select(look.arrow_brush);
  canvas.DrawPolygon(arrow, ARRAY_SIZE(arrow));
}


