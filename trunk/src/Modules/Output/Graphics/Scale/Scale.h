/*!
 * \file
 * \brief General definition for frequency scale management
 *
 * \author Willem van Engen <cnbh@willem.engen.nl>
 * \date created 2006/09/26
 * \version \$Id: Scale.h 576 2008-05-29 12:51:16Z tom $
 */
/* (c) 2006, University of Cambridge, Medical Research Council
 * http://www.pdn.cam.ac.uk/groups/cnbh/aimmanual
 */
#ifndef __MODULE_SCALE_H__
#define __MODULE_SCALE_H__

namespace aimc {

/*!
 * \class Scale "Modules/Scale/Scale.h"
 * \brief General class for frequency scale management
 *
 * This class
 *   - Converts between a linear and specified scale
 *   - Creates filterbank signals based on that scale
 *   - Creates any of the children filterbanks with Scale::Create.
 * To create a new scale, all you need to do is override ToLinear() and
 * FromLinear(). See existing scales like ScaleERB for example:
 */
class Scale;
class Scale {
public:
  //! \brief A list of possible scales
  enum ScaleType {
    SCALE_LINEAR, SCALE_ERB, SCALE_LOG, SCALE_LOGSCALED
  };
  /*! \brief Create a new scale based on type
   *  \param iType Scale type to create
   *  \param min Bottom frequency
   *  \param max Top frequency
   *  \param density Density of distribution on the scale (scale-dependent)
   *  \return Newly created scale, to be freed by the caller.
   *
   *  This is on purpose no virtual function, always use it Scale::Create().
   *  \todo Split into scaling and filterbank creation parts.
   *        Maybe even a separate ScaleMaker that implements Create().
   */
  static Scale *Create(ScaleType iType, unsigned int min, unsigned int max, float density);

  /*! \overload
   *  This function is for scaling-only Scales, you must not create a SignalBank
     *  with the returned Scale.
   */
  static Scale *Create(ScaleType iType);

  /*! \brief Create a new Scale
   *  \param min Bottom frequency
   *  \param max Top frequency
   *  \param density Density of distribution on the scale (scale-dependent)
   */
  Scale(unsigned int min, unsigned int max, float density) {
    m_iMin = min;
    m_iMax = max;
    m_fDensity = density;
    m_sName = NULL;
  };

  virtual ~Scale() { };

  /*! \brief Create an exact copy of this Scale
   *  \return A newly created Scale, to be freed by the caller.
   */
  virtual Scale *Clone();

  //! \return this Signal's ScaleType
  ScaleType getType() { return m_iType; };
  //! \return this Signal's name
  const char *getName() { return m_sName; };

  /*! \brief Scale a frequency from linear to this scale
   *  \param fFreq Frequency to scale
   *  \return Scaled frequency
   */
  virtual float FromLinear(float fFreq) = 0;

  /*! \brief Scale a frequency from this scale to linear
   *  \param fFreq Scaled frequency to scale back
   *  \return Linear frequency
   */
  virtual float ToLinear(float fFreq) = 0;

  /*! \brief Scale from [fMin..fMax] to [-0.5..0.5]
   *  \param fVal Value to scale, must be within [fMin..fMax]
   *  \return Float in [-0.5..0.5] according to this scale
   *  \sa FromLinearScaledExtrema
   *
   *  This is mainly for displaying any value on a scale and to make sure
   *  that it fits in on screen. Don't use this for any real maths!
   *
   *  The default implementation assumes that the scale is monotonic, so the
   *  FromLinear(fMin) and FromLinear(fMax) are scale's output boundaries.
   *
   *  fMin and fMax are set by FromLinearScaledExtrema. Do not use this before
   *  calling that.
   */
  float FromLinearScaled(float fVal);
  /*! \brief Update the FromLinearScaled min/max values
   *  \param fMin Minimum value to be input
   *  \param fMax Maxmimum value to be input
   *  \sa FromLinearScaled
   */
  void FromLinearScaledExtrema(float fMin, float fMax);
  /*! \overload
   *  \brief Copy min/max values from another Scale
   *  \param pScale Scale from which to copy min/max values, must not be NULL
   */
  void FromLinearScaledExtrema(Scale *pScale);

protected:
  //! \brief Bottom frequency
  unsigned int m_iMin;
  //! \brief Top frequency
  unsigned int m_iMax;
  //! \brief Density of distribution on the scale (scale-dependent)
  float m_fDensity;
  //! \brief The type of this scale, used by Clone(); set this in children.
  ScaleType m_iType;
  //! \brief The name of this scale; set this in children.
  const char *m_sName;

  /*! \brief Minimum value for scaling as input
   *  \sa FromLinearScaled(), FromLinearScaledExtrema() */
  float m_fMin;
  /*! \brief Maximum value for scaling as input
   *  \sa FromLinearScaled(), FromLinearScaledExtrema() */
  float m_fMax;
  //! \brief Value used in FromLinearScaled
  float m_fScaledCurHalfSum;
  //! \brief Value used in FromLinearScaled
  float m_fScaledCurDiff;
};
}  // namespace aimc
#endif /* __MODULE_SCALE_H__ */
