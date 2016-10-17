//-----------------------------------------------------------------------------
//
// This code is licensed to you under the terms of the GNU GPL, version 2 or,
// at your option, any later version. See the LICENSE.txt file for the text of
// the license.
//-----------------------------------------------------------------------------
// Low frequency T55xx commands
//-----------------------------------------------------------------------------
#ifndef CMDLFPYRAMID_H__
#define CMDLFPYRAMID_H__

int CmdLFPyramid(const char *Cmd);
int CmdPyramidClone(const char *Cmd);
int CmdPyramidSim(const char *Cmd);

int usage_lf_pyramid_clone(void);
int usage_lf_pyramid_sim(void);
#endif

