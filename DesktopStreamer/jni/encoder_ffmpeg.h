#ifdef __cplusplus
extern "C" {
#endif
void desktopStreamer_encoder_init(int *width, int *height);
void desktopStreamer_encoder_encodeFrame(RGBQUAD *rgb, int *w, int *h);
#ifdef __cplusplus
}
#endif
