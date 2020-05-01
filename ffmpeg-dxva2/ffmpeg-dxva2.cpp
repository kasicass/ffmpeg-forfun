// Win32Project1.cpp : ����Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "ffmpeg-dxva2.h"

#include "ffmpeg_dxva2.h"
#include "D3DVidRender.h"

#define MAX_LOADSTRING 100

// ȫ�ֱ���: 
HINSTANCE hInst;								// ��ǰʵ��
TCHAR szTitle[MAX_LOADSTRING];					// �������ı�
TCHAR szWindowClass[MAX_LOADSTRING];			// ����������

// �˴���ģ���а����ĺ�����ǰ������: 
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO:  �ڴ˷��ô��롣
	MSG msg;
	HACCEL hAccelTable;

	// ��ʼ��ȫ���ַ���
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_FFMPEGDXVA2, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// ִ��Ӧ�ó����ʼ��: 
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_FFMPEGDXVA2));

	// ����Ϣѭ��: 
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}

/*
int main()
{
	return _tWinMain(GetModuleHandle(NULL), NULL, GetCommandLine(), SW_NORMAL);
}*/


//
//  ����:  MyRegisterClass()
//
//  Ŀ��:  ע�ᴰ���ࡣ
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_FFMPEGDXVA2));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_FFMPEGDXVA2);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   ����:  InitInstance(HINSTANCE, int)
//
//   Ŀ��:  ����ʵ�����������������
//
//   ע��: 
//
//        �ڴ˺����У�������ȫ�ֱ����б���ʵ�������
//        ��������ʾ�����򴰿ڡ�
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // ��ʵ������洢��ȫ�ֱ�����

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

CD3DVidRender m_D3DVidRender;
AVPixelFormat GetHwFormat(AVCodecContext *s, const AVPixelFormat *pix_fmts)
{
	InputStream* ist = (InputStream*)s->opaque;
	ist->active_hwaccel_id = HWACCEL_DXVA2;
	ist->hwaccel_pix_fmt = AV_PIX_FMT_DXVA2_VLD;
	return ist->hwaccel_pix_fmt;
}

#define USE_LOGTRACE
#if defined(USE_LOGTRACE)
void LogTrace(LPCTSTR pszFormat, ...)  
{  
	va_list pArgs;  

	char szMessageBuffer[16380]={0};  
	va_start( pArgs, pszFormat );  
	_vsntprintf( szMessageBuffer, 16380, pszFormat, pArgs );  
	va_end( pArgs );  

	OutputDebugString(szMessageBuffer);  
}
#else
#define LogTrace(...)
#endif

bool g_bAccel = false;
void PlayVideo(HWND hWnd, const char* filename)
{
	DWORD start_time = GetTickCount();
	LogTrace("FFMPEG - start decoding, time = %d\n", start_time);

	int setup_hwdecode = 0;

	av_register_all();//ע�������
	//	av_log_set_level(AV_LOG_DEBUG);

	AVFormatContext *fc = NULL;
	int res = avformat_open_input(&fc, filename, NULL, NULL);//���ļ�
	if (res < 0) {
		LogTrace("FFMPEG - error %x in avformat_open_input\n", res);
		return;
	}

	res = avformat_find_stream_info(fc, NULL);//ȡ������Ϣ
	if (res < 0)
	{
		LogTrace("FFMPEG - error %x in avformat_find_stream_info\n", res);
		return;
	}

	//������Ƶ������Ƶ��
	av_dump_format(fc, 0, filename, 0);//�г������ļ����������Ϣ
	int i;
	int videoindex = -1;

	for (i = 0; i < fc->nb_streams; i++)
	{
		if (fc->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			videoindex = i;
		}
	}
	if (videoindex == -1)
	{
		LogTrace("FFMPEG - can't find video stream\n");
		return;

	}//û���ҵ���Ƶ�� 

	AVCodec *codec = avcodec_find_decoder(fc->streams[videoindex]->codec->codec_id);//��������Ϣ�ҵ�������
	if (!codec)
	{
		LogTrace("FFMPEG - decoder not found\n");
		return;
	}

	AVCodecContext *codecctx = fc->streams[videoindex]->codec;

	bool bAccel = g_bAccel;
	AVCodecContext *temp_codecctx = codecctx;
	memcpy(temp_codecctx, codecctx, sizeof(codecctx));

	if (bAccel && codecctx->codec_type == AVMEDIA_TYPE_VIDEO)
	{
		switch (codec->id)
		{
		case AV_CODEC_ID_MPEG2VIDEO:
		case AV_CODEC_ID_H264:
		case AV_CODEC_ID_VC1:
		case AV_CODEC_ID_WMV3:
		case AV_CODEC_ID_HEVC:
		case AV_CODEC_ID_VP9:
		{
			codecctx->thread_count = 1;  // Multithreading is apparently not compatible with hardware decoding
			InputStream *ist = new InputStream();
			ist->hwaccel_id = HWACCEL_AUTO;
			ist->active_hwaccel_id = HWACCEL_AUTO;
			ist->hwaccel_device = "dxva2";
			ist->dec = codec;
			ist->dec_ctx = codecctx;

			codecctx->opaque = ist;
			if (dxva2_init(codecctx, hWnd) == 0)
			{
				codecctx->get_buffer2 = ist->hwaccel_get_buffer;
				codecctx->get_format = GetHwFormat;
				codecctx->thread_safe_callbacks = 1;

				break;
			}
									
			bAccel = false;
			break;
		}
		default:
			bAccel = false;
			break;
		}
	}

	AVFrame	*pFrameBGR = NULL;
	uint8_t	*out_buffer = NULL;
	struct SwsContext *img_convert_ctx = NULL ;
	if (!bAccel)
	{
		avcodec_close(codecctx);
		codecctx = temp_codecctx;

		m_D3DVidRender.InitD3D_YUV(hWnd, codecctx->width, codecctx->height);

		pFrameBGR = av_frame_alloc();
		out_buffer = (uint8_t *)av_malloc(av_image_get_buffer_size(AV_PIX_FMT_YUV420P, codecctx->width, codecctx->height, 1));
		av_image_fill_arrays(pFrameBGR->data, ((AVPicture *)pFrameBGR)->linesize, out_buffer, AV_PIX_FMT_YUV420P, codecctx->width, codecctx->height, 1);
		img_convert_ctx = sws_getContext(codecctx->width, codecctx->height, codecctx->pix_fmt, codecctx->width, codecctx->height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);
	}

	res = avcodec_open2(codecctx, codec, NULL);
	if (res < 0) {
		LogTrace("FFMPEG - error %x in avcodec_open2\n", res);
		return;
	}

	AVPacket pkt = { 0 };
	AVFrame *picture = av_frame_alloc();
	DWORD wait_for_keyframe = 60;

	int count = 0;
						
	while (av_read_frame(fc, &pkt) == 0)
	{
		if (pkt.stream_index == videoindex)
		{
			int got_picture = 0;

			DWORD t_start = GetTickCount();
			int bytes_used = avcodec_decode_video2(codecctx, picture, &got_picture, &pkt);
			if (got_picture)
			{
				if (bAccel)
				{
					//��ȡ����ͬʱ��Ⱦ
					dxva2_retrieve_data_call(codecctx, picture);

					DWORD t_end = GetTickCount();
					LogTrace("FFMPEG - dxva2 time using: %lu\n", t_end - t_start);
				}
				else
				{
					if (img_convert_ctx &&pFrameBGR && out_buffer)
					{
						//ת�����ݲ���Ⱦ
						sws_scale(img_convert_ctx, (const uint8_t* const*)picture->data, picture->linesize, 0, codecctx->height, pFrameBGR->data, pFrameBGR->linesize);
						m_D3DVidRender.Render_YUV(out_buffer, picture->width, picture->height);

						DWORD t_end = GetTickCount();
						LogTrace("FFMPEG - normal time using: %lu\n", t_end - t_start);
					}
				}
									  
				count++;
			}

			av_packet_unref(&pkt);
		}
		else
		{
			av_packet_unref(&pkt);
		}
	}

	av_free(picture);

	avcodec_close(codecctx);
	avformat_close_input(&fc);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// �����˵�ѡ��: 
		switch (wmId)
		{
		case IDM_ABOUT:
			PlayVideo(hWnd, "D:\\test.mp4");
			break;

		case IDM_OPEN:
			{
				OPENFILENAME ofn;
				ZeroMemory(&ofn, sizeof(ofn));
				TCHAR szFileName[MAX_PATH];

				ofn.lpstrFile       = szFileName;
				ofn.lpstrFile[0]    = '\0';
				ofn.hwndOwner       = hWnd;
				ofn.lStructSize     = sizeof(OPENFILENAME);
				ofn.Flags           = OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST|OFN_NONETWORKBUTTON|OFN_NOCHANGEDIR;
				ofn.lpstrFilter     = _T("Video Files\0*.avi;*.mp4\0\0");
				// ofn.lpstrInitialDir = appDir_.c_str();
				ofn.nMaxFile        = MAX_PATH;
				ofn.lpstrTitle      = _T("Video File");

				if (::GetOpenFileName(&ofn) == TRUE)
				{
					PlayVideo(hWnd, ofn.lpstrFile);
				}
			}
			break;

		case IDM_ACCEL:
			g_bAccel = !g_bAccel;
			LogTrace("FFMPEG - dxva2: %s\n", g_bAccel ? "on" : "off");
			break;

		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO:  �ڴ���������ͼ����...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// �����ڡ������Ϣ�������
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
