/*
Copyright_License {

  XCSoar Glide Computer - http://www.xcsoar.org/
  Copyright (C) 2000-2010 The XCSoar Project
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

#include "NMEA/Info.hpp"
#include "NMEA/Derived.hpp"
#include "SettingsComputer.hpp"
#include "Device/NullPort.hpp"
#include "Device/Driver.hpp"
#include "Device/Register.hpp"
#include "Device/Parser.hpp"
#include "Device/device.hpp"
#include "Device/Geoid.h"
#include "Engine/Navigation/GeoPoint.hpp"
#include "Engine/GlideSolvers/GlidePolar.hpp"
#include "Engine/Waypoint/Waypoints.hpp"
#include "InputEvents.hpp"
#include "Thread/Trigger.hpp"
#include "BasicComputer.hpp"
#include "OS/PathName.hpp"
#include "Wind/WindZigZag.hpp"

#include <stdio.h>

const struct DeviceRegister *driver;

Waypoints way_points;

/*
 * Fake Device/device.cpp
 */

bool
HaveCondorDevice()
{
  return _tcscmp(driver->Name, _T("Condor")) == 0;
}

/*
 * Fake Device/Geoid.cpp
 */

fixed
LookupGeoidSeparation(const GeoPoint pt)
{
  return fixed_zero;
}

/*
 * Fake InputEvents.cpp
 */

bool
InputEvents::processNmea(unsigned key)
{
  return true;
}

/*
 * The actual code.
 */

int main(int argc, char **argv)
{
  if (argc != 2) {
    fprintf(stderr, "Usage: %s DRIVER\n"
            "Where DRIVER is one of:\n", argv[0]);

    const TCHAR *name;
    for (unsigned i = 0; (name = devRegisterGetName(i)) != NULL; ++i)
      _ftprintf(stderr, _T("\t%s\n"), name);

    return 1;
  }

  PathName driver_name(argv[1]);
  const struct DeviceRegister *driver = devGetDriver(driver_name);
  if (driver == NULL) {
    fprintf(stderr, "No such driver: %s\n", argv[1]);
    return 1;
  }

  NullPort port;
  Device *device = driver->CreateOnPort != NULL
    ? driver->CreateOnPort(&port)
    : NULL;

  NMEAParser parser;

  NMEA_INFO data, last;
  data.reset();

  static DERIVED_INFO calculated;
  static SETTINGS_COMPUTER settings_computer;

  BasicComputer computer;

  printf("# time quality wind_bearing (deg) wind_speed (m/s) grndspeed (m/s) tas (m/s) bearing (deg)\n");
  char buffer[1024];

  fixed speed(0);
  Angle bearing = Angle::native(fixed(0));

  while (fgets(buffer, sizeof(buffer), stdin) != NULL) {
    last = data;

    TrimRight(buffer);

    if (device == NULL || !device->ParseNMEA(buffer, &data))
      parser.ParseNMEAString_Internal(buffer, &data);

    computer.Compute(data, last, calculated, settings_computer);
    calculated.flight.flying_state_moving(data.Time);

    int quality = WindZigZagUpdate(data, calculated,
                                   speed, bearing);
    if (quality > 0)
      printf("%d %d %d %g %g %g %d\n", (int)data.Time, quality,
             (int)bearing.value_degrees(),
             (double)speed,
             (double)data.GroundSpeed,
             (double)data.TrueAirspeed,
             (int)data.TrackBearing.value_degrees());
  }
}

