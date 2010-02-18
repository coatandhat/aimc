/*!
 * \file
 * \brief File output to HTK format class definition
 *
 * \author Tom Walters <tcw24@cam.ac.uk> and Willem van Engen <cnbh@willem.engen.nl>
 * \date created 2006/10/30
 * \version \$Header$
 */
/* (c) 2006, University of Cambridge, Medical Research Council
 * http://www.pdn.cam.ac.uk/groups/cnbh/aimmanual
 */
#ifndef _AIMC_MODULE_OUTPUT_HTK_H_
#define _AIMC_MODULE_OUTPUT_HTK_H_

#include "Support/Module.h"
#include "Support/SignalBank.h"


// Defines taken from HTKwrite.c and The HTK Book
#define H_WAVEFORM		0	//sampled waveform
#define H_LPC			1	//linear prediction filter coefficients
#define H_LPREFC		2	//linear prediction reflection coefficients
#define H_LPCEPSTRA		3	//LPC cepstral coefficients
#define H_LPDELCEP		4	//LPC cepstra plus delta coefficients
#define H_IREFC			5	//LPC reflection coef in 16 bit integer format
#define H_MFCC			6	//mel-frequency cepstral coefficients
#define H_FBANK			7	//log mel-filter bank channel outputs
#define H_MELSPEC		8	//linear mel-filter bank channel outputs
#define H_USER			9	//user defined sample kind
#define H_DISCRETE		10	//vector quantised data
#define H_PLP           11  // Perceptual Linear Prediction
#define H_ANON          12

#define H_E 64 //has energy
#define H_N 128 //absolute energy suppressed
#define H_D 256 //has delta coefficients
#define H_A 512 //has acceleration coefficients
#define H_C 1024 //is compressed
#define H_Z 2048 //has zero mean static coef.
#define H_K 4096 //has CRC checksum
#define H_O 8192 //has 0th cepstral coef.
#define H_V 16384 // Attach vq index
#define H_T 32768 // Attach delta-delta-delta index

// HTK fomat is big-endian...
#define ByteSwap16(n) \
( ((((uint16_t) n) << 8) & 0xFF00) | \
((((uint16_t) n) >> 8) & 0x00FF) )

#define ByteSwap32(n) \
( ((((uint32_t) n) << 24) & 0xFF000000) | \
((((uint32_t) n) << 8) & 0x00FF0000) | \
((((uint32_t) n) >> 8) & 0x0000FF00) | \
((((uint32_t) n) >> 24) & 0x000000FF) )

/*!
 * \class FileOutputHTK "Output/FileOutputHTK.h"
 * \brief File output to HTK class
 *
 * This class gives a method for saving either a signal or a profile to HTK format.
 * \sa Signal, SignalBank
 */
namespace aimc {
class FileOutputHTK : public Module {
 public:
	/*! \brief Create a new file output for an HTK format file. Use of this 
	 *  class only really makes sense for the output of 1-D frames.
	 */
	FileOutputHTK(Parameters *pParam);
	~FileOutputHTK();

	/*! \brief Initialize the output to HTK.
	 *  \param *filename Filename of the ouptut file to be created.
	 *  If the file exists it will be overwritten
	 *  \return Returns true on success of initialization.
	 */
	bool OpenFile(const char *filename, float frame_period_ms);
  bool CloseFile();
  virtual void Process(const SignalBank &input);
protected:
  virtual bool InitializeInternal(const SignalBank &input);
  virtual void ResetInternal();

  float ByteSwapFloat(float d);

	void WriteHeader(int nelements, float sampPeriod);

	//! \brief Whether initialization is done or not
	bool header_written_;

	//! \brief Filename
	char filename_[PATH_MAX];
	//! \brief Internal pointer to the output file
	FILE *file_handle_;

	//! \brief Count of the number of samples in the file, written on close
	int sample_count_;

  int channel_count_;
  int buffer_length_;
  float frame_period_ms_;
};
}  // namespace aimc

#endif  // _AIMC_MODULE_OUTPUT_HTK_H_

