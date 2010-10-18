/**
 * @file
 *   Unless noted otherwise, the portions of Isis written by the USGS are public
 *   domain. See individual third-party library and package descriptions for
 *   intellectual property information,user agreements, and related information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or implied,
 *   is made by the USGS as to the accuracy and functioning of such software
 *   and related material nor shall the fact of distribution constitute any such
 *   warranty, and no responsibility is assumed by the USGS in connection
 *   therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html in a browser or see
 *   the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

#include "Distance.h"

#include "iException.h"
#include "iString.h"
#include "SpecialPixel.h"

namespace Isis {

  /**
   * This initializes the distance to an invalid state. You must set the
   *   distance later on with operator= or one of the Set methods.
   */
  Distance::Distance() {
    SetDistance(Null, Meters);
  }

  /**
   * This is the copy constructor for Distance. The distance passed in will be
   *   exactly duplicated.
   *
   * @param distanceToCopy This is the distance we are making an exact duplicate
   *    of
   */
  Distance::Distance(const Distance &distanceToCopy) {
    // Use meters because it is the stored format, no precision loss
    SetDistance(distanceToCopy.GetMeters(), Meters);
  }


  /**
   * This is the general purpose constructor for Distance. This will initialize
   *   with the given distance.
   *
   * @param distance The distance to initialize with, must be in units of
   *     distanceUnit and not be negative
   * @param distanceUnit The unit distance is in, can be any value in
   *     Distance::Units
   */
  Distance::Distance(double distance, Units distanceUnit) {
    SetDistance(distance, distanceUnit);
  }


  /**
   * This will free the memory allocated by this instance of the Distance class.
   */
  Distance::~Distance() {
    // This will help debug memory problems, better to reset to obviously bad
    //   values in case we're used after we're deleted. 
    p_distanceInMeters = Null;
  }


  /**
   * Get the distance in meters.
   *
   * @return Current distance, in meters, guaranteed to be >= 0.0
   */
  double Distance::GetMeters() const {
    return GetDistance(Meters);
  }


  /**
   * Set the distance in meters.
   *
   * @param distanceInMeters This is the value to set this distance to, given in
   *     meters. This will throw an exception if the value is negative.
   */
  void Distance::SetMeters(double distanceInMeters) {
    SetDistance(distanceInMeters, Meters);
  }


  /**
   * Get the distance in kilometers.
   *
   * @return Current distance, in kilometers, guaranteed to be >= 0.0
   */
  double Distance::GetKilometers() const {
    return GetDistance(Kilometers);
  }


  /**
   * Set the distance in kilometers.
   *
   * @param distanceInKilometers This is the value to set this distance to,
   *     given in kilometers. This will throw an exception if the value is 
   *     negative.
   */
  void Distance::SetKilometers(double distanceInKilometers) {
    SetDistance(distanceInKilometers, Kilometers);
  }


  /**
   * Test if this distance has been initialized or not
   *
   * @return True if this distance has been initialized.
   */
  bool Distance::Valid() const {
    return GetDistance(Meters) != Null;
  }


  /**
    * Compare two distances with the greater than operator.
    *
    * @param otherDistance This is the distance we're comparing to, i.e. on
    *     the right hand side of the operator when used
    * @return True if this distance is greater than the given distance
    */
  bool Distance::operator >(const Distance &otherDistance) const {
    if(!Valid() || !otherDistance.Valid()) {
      iString msg = "Distance has not been initialized, you must initialize "
          "it first before comparing with another distance using [>]";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    return GetMeters() > otherDistance.GetMeters();
  }


  /**
    * Compare two distances with the less than operator.
    *
    * @param otherDistance This is the distance we're comparing to, i.e. on
    *     the right hand side of the operator when used
    * @return True if this distance is less than the given distance
    */
  bool Distance::operator <(const Distance &otherDistance) const {
    if(!Valid() || !otherDistance.Valid()) {
      iString msg = "Distance has not been initialized, you must initialize "
          "it first before comparing with another distance using [<]";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    return GetMeters() < otherDistance.GetMeters();
  }


  /**
   * Assign this distance to the value of another distance.
   *
   * @param distanceToCopy This is the distance we are to duplicate exactly
   * @return Resulting distance, a reference to this distance after assignment
   */
  Distance &Distance::operator =(const Distance &distanceToCopy) {
    if(this == &distanceToCopy) return *this;

    SetDistance(distanceToCopy.GetMeters(), Meters);

    return *this;
  }


  /**
   * Add another distance to this distance (1km + 1m = 1005m)
   *
   * @param distanceToAdd This is the distance we are adding to ourselves
   * @return Resulting distance, self not modified
   */
  Distance Distance::operator +(const Distance &distanceToAdd) const {
    if(!Valid() || !distanceToAdd.Valid()) return Distance();

    return Distance(GetMeters() + distanceToAdd.GetMeters(), Meters);
  }


  /**
   * Subtract another distance from this distance (1km - 1m = 995m). This could
   *   throw an exception if the result is negative.
   *
   * @param distanceToSub This is the distance we are subtracting from ourself
   * @return Resulting distance, self not modified
   */
  Distance Distance::operator -(const Distance &distanceToSub) const {
    Distance result(GetMeters() - distanceToSub.GetMeters(), Meters);
    return result;
  }


  /**
   * Add and assign the given distance to ourselves.
   *
   * @param distanceToAdd This is the distance we are to duplicate exactly
   * @return Resulting distance, a reference to this distance after assignment
   */
  void Distance::operator +=(const Distance &distanceToAdd) {
    SetDistance(GetMeters() + distanceToAdd.GetMeters(), Meters);
  }


  /**
   * Subtract and assign the given distance from ourself. This could throw
   *   an exception if the result is negative, in which case the new value is
   *   never applied.
   *
   * @param distanceToSub This is the distance we are to duplicate exactly
   * @return Resulting distance, a reference to this distance after assignment
   */
  void Distance::operator -=(const Distance &distanceToSub) {
    SetDistance(GetMeters() - distanceToSub.GetMeters(), Meters);
  }


  /**
   * This is a helper method to access distances in a universal manner with
   *   uniform error checking.
   *
   * @param distanceUnit Unit of the return value. If this is invalid, an
   *     exception will be thrown.
   * @return The distance in units of distanceUnit
   */
  double Distance::GetDistance(Units distanceUnit) const {
    double distanceInMeters = p_distanceInMeters;
    double resultingDistance = Null;

    if(p_distanceInMeters == Null) return Null;

    switch(distanceUnit) {
      case Meters:
        resultingDistance = distanceInMeters;
        break;

      case Kilometers:
        resultingDistance = distanceInMeters / 1000.0;
        break;
    }

    if(resultingDistance == Null) {
      iString msg = "Distance does not understand the enumerated value [" +
        iString(distanceUnit) + "] as a unit";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    return resultingDistance;
  }


  /**
   * This is a helper method to set distances in a universal manner with
   *   uniform error checking.
   *
   * @param distance The distance, in units of distanceUnit, to set this class
   *     to. If this is negative an exception will be thrown and the state
   *     unmodified.
   * @param distanceUnit Unit of distance. If this is invalid, an
   *     exception will be thrown and the state left unmodified.
   */
  void Distance::SetDistance(const double &distance, Units distanceUnit) {
    double distanceInMeters = Null;

    if(distance == Null) {
      p_distanceInMeters = Null;
      return;
    }

    switch(distanceUnit) {
      case Meters:
        distanceInMeters = distance;
        break;

      case Kilometers:
        distanceInMeters = distance * 1000.0;
        break;
    }

    if(distanceInMeters == Null) {
      iString msg = "Distance does not understand the enumerated value [" +
        iString(distanceUnit) + "] as a unit";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    if (distanceInMeters < 0.0) {
      iString msg = "Negative distances are not supported, the value [" +
        iString(distanceInMeters) + " meters] cannot be stored in the Distance "
        "class";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    p_distanceInMeters = distanceInMeters;
  }
}
