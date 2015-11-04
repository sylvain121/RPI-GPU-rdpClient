#ifdef __cplusplus
extern "C" {
#endif
int x11_init (int *width, int *height, int *depth);
void x11_getNextFrame(RGBQUAD *rgb, int *width, int *height);
#ifdef __cplusplus
}
#endif
