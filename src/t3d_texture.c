/*----- T3D_texture.c --------------------------------------------------------+
    Developped by: Edward LEI
    Revised date: 18th Jan, 2014
 +----------------------------------------------------------------------------*/

#include <stdlib.h>
#include <math.h>
#include <stb_image.h>
#include <t3d_ogl.h>
#include <t3d_texture.h>


static int
convert_bit_range( const int c, const int from_bits, const int to_bits )
{
  int b = (1 << (from_bits - 1)) + c*((1 << to_bits) - 1);
  return (b + (b >> from_bits)) >> from_bits;
}

static int
rgb_to_565( const int r, const int g, const int b )
{
  return (convert_bit_range(r, 8, 5) << 11) |
         (convert_bit_range(g, 8, 6) << 05) |
         (convert_bit_range(b, 8, 5) << 00);
}

static void rgb_888_from_565( const unsigned int c, int *r, int *g, int *b )
{
	*r = convert_bit_range((c >> 11) & 31, 5, 8);
	*g = convert_bit_range((c >> 05) & 63, 6, 8);
	*b = convert_bit_range((c >> 00) & 31, 5, 8);
}

static void
compute_color_line_STDEV( const unsigned char *const uncompressed,
                          const int channels,
                          float point[3], float direction[3] )
{
  const float inv_16 = 1.0/16.0;
  int i;
  float sum_r  = 0.0, sum_g  = 0.0, sum_b  = 0.0;
  float sum_rr = 0.0, sum_gg = 0.0, sum_bb = 0.0;
  float sum_rg = 0.0, sum_rb = 0.0, sum_gb = 0.0;
  /* calculate all data needed for the covariance matrix (to compare with _rygdxt code) */
  for(i = 0; i < 16*channels; i += channels) {
    sum_r  += uncompressed[i + 0];
    sum_rr += uncompressed[i + 0]*uncompressed[i + 0];
    sum_g  += uncompressed[i + 1];
    sum_gg += uncompressed[i + 1]*uncompressed[i + 1];
    sum_b  += uncompressed[i + 2];
    sum_bb += uncompressed[i + 2]*uncompressed[i + 2];
    sum_rg += uncompressed[i + 0]*uncompressed[i + 1];
    sum_rb += uncompressed[i + 0]*uncompressed[i + 2];
    sum_gb += uncompressed[i + 1]*uncompressed[i + 2];
  }
  /* convert the sums to averages */
  sum_r *= inv_16;
  sum_g *= inv_16;
  sum_b *= inv_16;
  /* and convert the squares to the squares of the value - avg_value	*/
  sum_rr -= 16.0*sum_r*sum_r;
  sum_gg -= 16.0*sum_g*sum_g;
  sum_bb -= 16.0*sum_b*sum_b;
  sum_rg -= 16.0*sum_r*sum_g;
  sum_rb -= 16.0*sum_r*sum_b;
  sum_gb -= 16.0*sum_g*sum_b;
  /* the point on the color line is the average */
  point[0] = sum_r;
  point[1] = sum_g;
  point[2] = sum_b;
  #if USE_COV_MAT
  /*
    The following idea was from ryg.
    (https://mollyrocket.com/forums/viewtopic.php?t=392)
    The method worked great (less RMSE than mine) most of
    the time, but had some issues handling some simple
    boundary cases, like full green next to full red,
    which would generate a covariance matrix like this:

    |  1  -1  0 |
    | -1   1  0 |
    |  0   0  0 |

    For a given starting vector, the power method can
    generate all zeros!  So no starting with {1,1,1}
    as I was doing!  This kind of error is still a
    slight posibillity, but will be very rare.
  */
  /* use the covariance matrix directly (1st iteration, don't use all 1.0 values!) */
  sum_r = 1.0;
  sum_g = 2.718281828;
  sum_b = 3.141592654;
  direction[0] = sum_r*sum_rr + sum_g*sum_rg + sum_b*sum_rb;
  direction[1] = sum_r*sum_rg + sum_g*sum_gg + sum_b*sum_gb;
  direction[2] = sum_r*sum_rb + sum_g*sum_gb + sum_b*sum_bb;
  /* 2nd iteration, use results from the 1st guy */
  sum_r = direction[0];
  sum_g = direction[1];
  sum_b = direction[2];
  direction[0] = sum_r*sum_rr + sum_g*sum_rg + sum_b*sum_rb;
  direction[1] = sum_r*sum_rg + sum_g*sum_gg + sum_b*sum_gb;
  direction[2] = sum_r*sum_rb + sum_g*sum_gb + sum_b*sum_bb;
  /* 3rd iteration, use results from the 2nd guy */
  sum_r = direction[0];
  sum_g = direction[1];
  sum_b = direction[2];
  direction[0] = sum_r*sum_rr + sum_g*sum_rg + sum_b*sum_rb;
  direction[1] = sum_r*sum_rg + sum_g*sum_gg + sum_b*sum_gb;
  direction[2] = sum_r*sum_rb + sum_g*sum_gb + sum_b*sum_bb;
	#else
  /* use my standard deviation method (very robust, a tiny bit slower and less accurate) */
  direction[0] = sqrtf(sum_rr);
  direction[1] = sqrtf(sum_gg);
  direction[2] = sqrtf(sum_bb);
  /* which has a greater component */
  if(sum_gg > sum_rr) {
    /* green has greater component, so base the other signs off of green */
    if(sum_rg < 0.0)  direction[0] = -direction[0];
    if(sum_gb < 0.0)  direction[2] = -direction[2];
  }
  else {
    /* red has a greater component */
    if(sum_rg < 0.0)  direction[1] = -direction[1];
		if(sum_rb < 0.0)  direction[2] = -direction[2];
  }
  #endif
}

static void
LSE_master_colors_max_min( int *cmax, int *cmin,
                           const int channels,
                           const unsigned char *const uncompressed )
{
  int i, j;
  /* the master colors */
  int c0[3], c1[3];
  /* used for fitting the line */
  float sum_x[] = {0.0, 0.0, 0.0};
  float sum_x2[] = {0.0, 0.0, 0.0};
  float dot_max = 1.0, dot_min = -1.0;
  float vec_len2 = 0.0;
  float dot;
  /* error check */
  if((channels < 3) || (channels > 4))
    return;

  compute_color_line_STDEV(uncompressed, channels, sum_x, sum_x2);
  vec_len2 = 1.0/(0.00001 + sum_x2[0]*sum_x2[0] + sum_x2[1]*sum_x2[1] + sum_x2[2]*sum_x2[2]);
  /* finding the max and min vector values */
  dot_max = sum_x2[0]*uncompressed[0] +
            sum_x2[1]*uncompressed[1] +
            sum_x2[2]*uncompressed[2];
  dot_min = dot_max;
  for(i = 1; i < 16; ++i) {
    dot = sum_x2[0]*uncompressed[i*channels + 0] +
          sum_x2[1]*uncompressed[i*channels + 1] +
          sum_x2[2]*uncompressed[i*channels + 2];
    if(dot < dot_min) {
      dot_min = dot;
    }
    else if(dot > dot_max) {
      dot_max = dot;
    }
  }
  /* and the offset (from the average location) */
  dot = sum_x2[0]*sum_x[0] + sum_x2[1]*sum_x[1] + sum_x2[2]*sum_x[2];
  dot_min -= dot;
  dot_max -= dot;
  /* post multiply by the scaling factor */
  dot_min *= vec_len2;
  dot_max *= vec_len2;
  /* OK, build the master colors */
  for(i = 0; i < 3; ++i) {
    /* color 0 */
    c0[i] = (int)(0.5 + sum_x[i] + dot_max*sum_x2[i]);
    if(c0[i] < 0) {
      c0[i] = 0;
    }
    else if(c0[i] > 255) {
      c0[i] = 255;
    }
    /* color 1 */
    c1[i] = (int)(0.5 + sum_x[i] + dot_min*sum_x2[i]);
    if(c1[i] < 0) {
      c1[i] = 0;
    }
    else if(c1[i] > 255) {
      c1[i] = 255;
    }
  }
  /* down_sample (with rounding?) */
  i = rgb_to_565(c0[0], c0[1], c0[2]);
  j = rgb_to_565(c1[0], c1[1], c1[2]);
  if(i > j) {
    *cmax = i;
    *cmin = j;
  }
  else {
    *cmax = j;
    *cmin = i;
  }
}

static void
compress_DDS_color_block( const int channels,
                          const unsigned char *const uncompressed,
                          unsigned char compressed[8] )
{
  /* variables */
  int i;
  int next_bit;
  int enc_c0 = 0, enc_c1 = 0;
  int c0[4], c1[4];
  float color_line[] = {0.0, 0.0, 0.0, 0.0};
  float vec_len2 = 0.0, dot_offset = 0.0;
  /* stupid order */
  int swizzle4[] = {0, 2, 3, 1};
  /* get the master colors */
  LSE_master_colors_max_min(&enc_c0, &enc_c1, channels, uncompressed);
  /* store the 565 color 0 and color 1 */
  compressed[0] = (enc_c0 >> 0) & 255;
  compressed[1] = (enc_c0 >> 8) & 255;
  compressed[2] = (enc_c1 >> 0) & 255;
  compressed[3] = (enc_c1 >> 8) & 255;
  /* zero out the compressed data */
  compressed[4] = 0;
  compressed[5] = 0;
  compressed[6] = 0;
  compressed[7] = 0;
  /* reconstitute the master color vectors */
  rgb_888_from_565(enc_c0, &c0[0], &c0[1], &c0[2]);
  rgb_888_from_565(enc_c1, &c1[0], &c1[1], &c1[2]);
  /* the new vector */
  vec_len2 = 0.0;
  for(i = 0; i < 3; ++i) {
    color_line[i] = (float)(c1[i] - c0[i]);
    vec_len2 += color_line[i] * color_line[i];
  }
	if(vec_len2 > 0.0) {
    vec_len2 = 1.0/vec_len2;
  }
  /* pre-proform the scaling */
  color_line[0] *= vec_len2;
  color_line[1] *= vec_len2;
  color_line[2] *= vec_len2;
  /* compute the offset (constant) portion of the dot product	*/
  dot_offset = color_line[0]*c0[0] + color_line[1]*c0[1] + color_line[2]*c0[2];
  /* store the rest of the bits	*/
  next_bit = 8*4;
  for(i = 0; i < 16; ++i) {
    /* find the dot product of this color, place it on the line (should be [-1,1])	*/
    int next_value = 0;
    float dot_product = color_line[0]*uncompressed[i*channels + 0] +
                        color_line[1]*uncompressed[i*channels + 1] +
                        color_line[2]*uncompressed[i*channels + 2] - dot_offset;
    /* map to [0,3] */
    next_value = (int)(dot_product*3.0 + 0.5);
    if(next_value > 3) {
      next_value = 3;
    }
    else if(next_value < 0) {
      next_value = 0;
    }
    /* OK, store this value */
    compressed[next_bit >> 3] |= swizzle4[next_value] << (next_bit & 7);
    next_bit += 2;
  }
}

static void
compress_DDS_alpha_block( const unsigned char *const uncompressed,
                          unsigned char compressed[8] )
{
  /* variables */
  int i;
  int next_bit;
  int a0, a1;
  float scale_me;
  /* stupid order */
  int swizzle8[] = { 1, 7, 6, 5, 4, 3, 2, 0 };
  /* get the alpha limits (a0 > a1) */
  a0 = a1 = uncompressed[3];
  for(i = 4+3; i < 16*4; i += 4) {
    if(uncompressed[i] > a0) {
      a0 = uncompressed[i];
    }
    else if(uncompressed[i] < a1) {
      a1 = uncompressed[i];
    }
  }
  /* store those limits, and zero the rest of the compressed dataset */
  compressed[0] = a0;
  compressed[1] = a1;
  /* zero out the compressed data */
  compressed[2] = 0;
  compressed[3] = 0;
  compressed[4] = 0;
  compressed[5] = 0;
  compressed[6] = 0;
  compressed[7] = 0;
  /* store the all of the alpha values */
  next_bit = 8*2;
  scale_me = 7.9999/(a0 - a1);
  for(i = 3; i < 16*4; i += 4) {
    /* convert this alpha value to a 3 bit number */
    int svalue;
    int value = (int)((uncompressed[i] - a1)*scale_me);
    svalue = swizzle8[value&7];
    /* OK, store this value, start with the 1st byte */
    compressed[next_bit >> 3] |= svalue << (next_bit & 7);
    if((next_bit & 7) > 5) {
      /* spans 2 bytes, fill in the start of the 2nd byte */
      compressed[1 + (next_bit >> 3)] |= svalue >> (8 - (next_bit & 7));
    }
    next_bit += 3;
  }
}

static unsigned char * 
convert_img_to_DXT1( const unsigned char *const uncompressed,
                     const int width, const int height, const int channels,
                     int *out_size )
{
  unsigned char *compressed;
  int i, j, x, y;
  unsigned char ublock[16*3];
  unsigned char cblock[8];
  int index = 0, chan_step = 1;
  int block_count = 0;
  /* error check */
  *out_size = 0;
  if((width < 1) || (height < 1)|| (NULL == uncompressed) ||
     (channels < 1) || (channels > 4)) {
		return NULL;
  }
  /* for channels == 1 or 2, I do not step forward for R,G,B values	*/
  if(channels < 3) {
    chan_step = 0;
  }
  /* get the RAM for the compressed image (8 bytes per 4x4 pixel block) */
  *out_size = ((width + 3) >> 2)*((height + 3) >> 2)*8;
  compressed = (unsigned char *)malloc(*out_size);
  /* go through each block */
  for(j = 0; j < height; j += 4) {
    for(i = 0; i < width; i += 4) {
      /* copy this block into a new one */
      int idx = 0;
      int mx = 4, my = 4;
      if(j + 4 >= height)  my = height - j;
      if(i + 4 >= width)   mx = width - i;

      for(y = 0; y < my; ++y) {
        for(x = 0; x < mx; ++x) {
          ublock[idx++] = uncompressed[(j + y)*width*channels + (i +x )*channels];
          ublock[idx++] = uncompressed[(j + y)*width*channels + (i +x )*channels + chan_step];
          ublock[idx++] = uncompressed[(j + y)*width*channels + (i +x )*channels + chan_step + chan_step];
        }
        for(x = mx; x < 4; ++x) {
          ublock[idx++] = ublock[0];
          ublock[idx++] = ublock[1];
          ublock[idx++] = ublock[2];
        }
      }
      for(y = my; y < 4; ++y) {
        for(x = 0; x < 4; ++x) {
          ublock[idx++] = ublock[0];
          ublock[idx++] = ublock[1];
          ublock[idx++] = ublock[2];
        }
      }
      /* compress the block */
      ++block_count;
      compress_DDS_color_block(3, ublock, cblock);
      /* copy the data from the block into the main block */
      for(x = 0; x < 8; ++x) {
        compressed[index++] = cblock[x];
      }
    }
  }
  return compressed;
}

static unsigned char *
convert_img_to_DXT5( const unsigned char *const uncompressed,
                     const int width, const int height, const int channels,
                     int *out_size )
{
  unsigned char *compressed;
  int i, j, x, y;
  unsigned char ublock[16*4];
  unsigned char cblock[8];
  int index = 0, chan_step = 1;
  int block_count = 0, has_alpha;
  /* error check */
	*out_size = 0;
  if((width < 1) || (height < 1) || (NULL == uncompressed) ||
     (channels < 1) || ( channels > 4)) {
    return NULL;
  }
  /* for channels == 1 or 2, I do not step forward for R,G,B vales */
  if(channels < 3) {
    chan_step = 0;
  }
  /* # channels = 1 or 3 have no alpha, 2 & 4 do have alpha */
  has_alpha = 1 - (channels & 1);
  /* get the RAM for the compressed image (16 bytes per 4x4 pixel block) */
  *out_size = ((width + 3) >> 2)*((height + 3) >> 2)*16;
  compressed = (unsigned char *)malloc(*out_size);
  /* go through each block */
  for(j = 0; j < height; j += 4) {
    for(i = 0; i < width; i += 4) {
      /* local variables, and my block counter */
      int idx = 0;
      int mx = 4, my = 4;
      if(j + 4 >= height)  my = height - j;
			if(i + 4 >= width)   mx = width - i;

      for(y = 0; y < my; ++y) {
        for(x = 0; x < mx; ++x) {
          ublock[idx++] = uncompressed[(j + y)*width*channels + (i + x)*channels];
          ublock[idx++] = uncompressed[(j + y)*width*channels + (i + x)*channels + chan_step];
          ublock[idx++] = uncompressed[(j + y)*width*channels + (i + x)*channels + chan_step + chan_step];
          ublock[idx++] = uncompressed[(j + y)*width*channels + (i + x)*channels + channels - 1]*has_alpha + (1 - has_alpha)*255;
        }
        for(x = mx; x < 4; ++x) {
          ublock[idx++] = ublock[0];
          ublock[idx++] = ublock[1];
          ublock[idx++] = ublock[2];
          ublock[idx++] = ublock[3];
        }
      }
      for(y = my; y < 4; ++y) {
        for(x = 0; x < 4; ++x) {
          ublock[idx++] = ublock[0];
          ublock[idx++] = ublock[1];
          ublock[idx++] = ublock[2];
          ublock[idx++] = ublock[3];
        }
      }
      /* now compress the alpha block */
      compress_DDS_alpha_block(ublock, cblock);
      /* copy the data from the compressed alpha block into the main buffer */
      for(x = 0; x < 8; ++x) {
        compressed[index++] = cblock[x];
      }
      /* then compress the color block */
      ++block_count;
      compress_DDS_color_block(4, ublock, cblock);
      /* copy the data from the compressed color block into the main buffer */
      for(x = 0; x < 8; ++x) {
        compressed[index++] = cblock[x];
      }
    }
  }
  return compressed;
}

/*	Upscaling the image uses simple bilinear interpolation	*/
static void
upscale_img( const unsigned char* const orig,
             const int width, const int height, const int channels,
             unsigned char *resampled,
             const int resampled_width, const int resampled_height )
{
  float dx, dy;
  int x, y, c;

  /* error(s) check */
  if((width < 1) || (height < 1) || (channels < 1) ||
     (resampled_width < 2) || (resampled_height < 2) ||
     (NULL == orig) || (NULL == resampled)) {
    /* signify badness */
    return;
  }
  /* for each given pixel in the new map, find the exact location
     from the original map which would contribute to this guy */
  dx = (width - 1.0)/(resampled_width - 1.0);
  dy = (height - 1.0)/(resampled_height - 1.0);
  for(y = 0; y < resampled_height; ++y) {
    /* find the base y index and fractional offset from that */
    float sampley = y*dy;
    int inty = (int)sampley;
    /* if(inty < 0) { inty = 0; } else*/
    if(inty > height - 2) { inty = height - 2;}
    sampley -= inty;
    for(x = 0; x < resampled_width; ++x) {
      float samplex = x*dx;
      int intx = (int)samplex;
      int base_index;
      /* find the base x index and fractional offset from that */
      /* if(intx < 0) { intx = 0; } else */
      if(intx > width - 2) { intx = width - 2; }
      samplex -= intx;
      /* base index into the original image */
      base_index = (inty*width + intx)*channels;
      for(c = 0; c < channels; ++c) {
        /* do the sampling */
        float value = 0.5;
        value += orig[base_index]*(1.0 - samplex)*(1.0 - sampley);
        value += orig[base_index + channels]*(samplex)*(1.0 - sampley);
        value += orig[base_index + width*channels]*(1.0 - samplex)*(sampley);
        value += orig[base_index + width*channels + channels]*(samplex)*(sampley);
        /* move to the next channel */
        ++base_index;
        /* save the new value */
        resampled[y*resampled_width*channels + x*channels + c] = (unsigned char)(value);
      }
    }
  }
}

static void
mipmap_img( const unsigned char* const orig,
            const int width, const int height, const int channels,
            unsigned char* resampled,
            const int block_size_x, const int block_size_y )
{
  int mip_w, mip_h;
  int i, j, c;

  /* error check */
  if((width < 1) || (height < 1) ||
     (channels < 1) || (orig == NULL) ||
     (resampled == NULL) ||
     (block_size_x < 1) || (block_size_y < 1) ) {
    /* nothing to do */
		return;
  }
  mip_w = width/block_size_x;
  mip_h = height/block_size_y;
  if(mip_w < 1)  mip_w = 1;
	if(mip_h < 1)  mip_h = 1;

  for(j = 0; j < mip_h; ++j) {
    for(i = 0; i < mip_w; ++i) {
      for(c = 0; c < channels; ++c) {
        const int index = (j*block_size_y)*width*channels + (i*block_size_x)*channels + c;
        int sum_value;
        int u, v;
        int u_block = block_size_x;
        int v_block = block_size_y;
        int block_area;
        /* check so we don't over-run the boundaries (necessary for non-square textures!)	*/
        if(block_size_x*(i + 1) > width) {
          u_block = width - i*block_size_y;
        }
        if(block_size_y*(j + 1) > height) {
          v_block = height - j*block_size_y;
        }
        block_area = u_block*v_block;
        /* for this pixel, see what the average of all the values in the block are */
        /* note: start the sum at the rounding value, not at 0 */
        sum_value = block_area >> 1;
        for(v = 0; v < v_block; ++v) {
          for(u = 0; u < u_block; ++u) {
            sum_value += orig[index + v*width*channels + u*channels];
          }
        }
        resampled[j*mip_w*channels + i*channels + c] = sum_value / block_area;
      }
    }
  }
}

#define GL_TEXTURE_MAX_ANISOTROPY_EXT 0x84FE
#define GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT 0x84FF
/* set texture flags */
void
tex_set_flags( const GLuint tex_id, const GLuint flags )
{
  /* bind an OpenGL texture ID */
  glBindTexture(GL_TEXTURE_2D, tex_id);

  if(flags & TEX_USE_NEAREST) {
    /* always use NEAREST for shadow maps for GL_TEXTURE_MIN_FILTER */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  }

  if(flags & TEX_USE_LINEAR) {
    /* instruct OpenGL _NOT_ to use the MIPmaps */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  }

  if(flags & TEX_USE_MIPMAPS) {
    /* instruct OpenGL to use the MIPmaps */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  }

  /* does the user want clamping, or wrapping? */
  if(flags & TEX_REPEATS) {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  }

  if(flags & TEX_TO_EDGE) {
    /* GL_CLAMP, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  }

  if(flags & TEX_TO_BORDER) {
    /* GL_CLAMP, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  }

  float maxAnisotropy;
  glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy);
}

/* not in glcorearb.h, in gl.h */
#define GL_CLAMP                          0x2900
#define GL_LUMINANCE                      0x1909
#define GL_LUMINANCE_ALPHA                0x190A
/* not in glcorearb.h, in glext.h */
#define GL_COMPRESSED_LUMINANCE           0x84EA
#define GL_COMPRESSED_LUMINANCE_ALPHA     0x84EB
/* not in glcorearb.h, in glext.h */
#define GL_COMPRESSED_RGB_S3TC_DXT1_EXT   0x83F0
#define GL_COMPRESSED_RGBA_S3TC_DXT5_EXT  0x83F3
/* Loads an image from disk into an OpenGL texture */
GLuint
tex_load( const char *file, const int s3tc_compressed )
{
  /* variables	*/
  unsigned char *img;
  GLuint tex_id = 0;
  GLuint internal_format = 0, original_format = 0;
  int w, h;    /* width, height */
  int channels;

  /* create a copy the image data */
  img = stbi_load(file, &w, &h, &channels, 0);

  int new_w = 1;
  int new_h = 1;
  while(new_w < w) {
    new_w *= 2;
  }
  while(new_h < h) {
    new_h *= 2;
  }

  if((new_w != w) || (new_h != h)) {
    /* yep, resize */
    unsigned char *resampled = (unsigned char *)malloc(channels*new_w*new_h);
    upscale_img(img, w, h, channels, resampled, new_w, new_h);
    /* nuke the old guy, then point it at the new guy */
    stbi_image_free(img);
    img = resampled;
    w = new_w;
    h = new_h;
  }

  /* create the OpenGL texture ID handle */
  glGenTextures(1, &tex_id);
  /* Note: sometimes glGenTextures fails (usually no OpenGL context)	*/
  if(tex_id) {
    /* use opengl standard compression by default */
    switch(channels) {
      case 1:
        original_format = GL_RED;
        internal_format = GL_RED;
        break;
      case 2:
        original_format = GL_LUMINANCE_ALPHA;
        internal_format = GL_COMPRESSED_LUMINANCE_ALPHA;
        break;
      case 3:
        original_format = GL_RGB;
        internal_format = GL_COMPRESSED_RGB;
        break;
      case 4:
        original_format = GL_RGBA;
        internal_format = GL_COMPRESSED_RGBA;
        break;
    }
    /* bind an OpenGL texture ID */
    glBindTexture(GL_TEXTURE_2D, tex_id);
    /* does the user want to use DXT? */
    if(s3tc_compressed) {
      int DDS_size;
      unsigned char *DDS_data = NULL;
      if((channels & 1) == 1) {
        /* RGB, use DXT1 */
        internal_format = GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
        DDS_data = convert_img_to_DXT1(img, w, h, channels, &DDS_size);
      }
      else {
        /* RGBA, use DXT5 */
        internal_format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
        DDS_data = convert_img_to_DXT5(img, w, h, channels, &DDS_size);
      }
      if(DDS_data) {
        /* printf("Internal DXT compressor\n"); */
        glCompressedTexImage2D(GL_TEXTURE_2D, 0,
                               internal_format, w, h, 0,
                               DDS_size, DDS_data);
        free(DDS_data);
      }
    }
    else {
      /* user want OpenGL to do all the work! */
      glTexImage2D(GL_TEXTURE_2D, 0,
                   internal_format, w, h, 0,
                   original_format, GL_UNSIGNED_BYTE, img);
    }

    /* default: TEX_USE_MIPMAPS */
    int miplevel = 1;
    int mip_w = (w + 1)/2;
    int mip_h = (h + 1)/2;
    unsigned char *resampled = (unsigned char *)malloc(channels*mip_w*mip_h);
    while(((1<<miplevel) <= w) || ((1<<miplevel) <= h)) {
      /* do this MIPmap level */
      mipmap_img(img, w, h, channels, resampled, (1 << miplevel), (1 << miplevel));

		  if(s3tc_compressed) {
        int DDS_size;
        unsigned char *DDS_data = NULL;
        if((channels & 1) == 1) {
          /* RGB, use DXT1 */
          DDS_data = convert_img_to_DXT1(resampled, mip_w, mip_h, channels, &DDS_size);
        }
        else {
          /* RGBA, use DXT5 */
          DDS_data = convert_img_to_DXT5(resampled, mip_w, mip_h, channels, &DDS_size);
        }
        if(DDS_data) {
          glCompressedTexImage2D(GL_TEXTURE_2D, miplevel,
                                 internal_format, mip_w, mip_h, 0,
                                 DDS_size, DDS_data);
          free(DDS_data);
        }
      }
      else {
        /* user want OpenGL to do all the work! */
        glTexImage2D(GL_TEXTURE_2D, miplevel,
                     internal_format, mip_w, mip_h, 0,
                     original_format, GL_UNSIGNED_BYTE, resampled);
      }
      /* prep for the next level */
      ++miplevel;
      mip_w = (mip_w + 1)/2;
      mip_h = (mip_h + 1)/2;
    }
    free(resampled);
    /* instruct OpenGL to use the MIPmaps */
    /* glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); */
    /* glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); */

    /* default: TEX_REPEATS */
    /* glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); */
    /* glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); */
  }
  stbi_image_free(img);

  return tex_id;
}

/* generate an GL_RED texture */
GLuint
tex_gen_red_tex( const unsigned char *texels, const int tex_w, const int tex_h )
{
  GLuint tex_id;

  glGenTextures(1, &tex_id);
  glBindTexture(GL_TEXTURE_2D, tex_id);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, tex_w, tex_h, 0, GL_RED, GL_UNSIGNED_BYTE, texels);

  return tex_id;
}