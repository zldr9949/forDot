#include <opencv2\core\core.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\highgui\highgui.hpp>

// 初期DOTサイズ
#define DEFAULT_DOT_SIZE 5
#define MAX_DOT_SIZE 40
// 色階調数　COLORSの三乗が色数
#define COLORS 4
#define MAX_COLORS 256

// リサイズ処理
void ResizeImage(cv::Mat &inputMat,cv::Mat &outputMat,int dotSize = DEFAULT_DOT_SIZE) {
	int fixY = (inputMat.size().height % dotSize);
	int fixX = (inputMat.size().width % dotSize);
	if (fixY != 0) {
		// バイキュービック法でのリサイズ
		if (fixX != 0) {
			cv::resize(inputMat, outputMat, cv::Size(inputMat.size().width - fixX, inputMat.size().height - fixY), CV_INTER_CUBIC);
		} else {
			cv::resize(inputMat, outputMat, cv::Size(inputMat.size().width, inputMat.size().height - fixY), CV_INTER_CUBIC);
		}
	} else if (fixX != 0) {
		cv::resize(inputMat, outputMat, cv::Size(inputMat.size().width - fixX, inputMat.size().height), CV_INTER_CUBIC);
	}
}

/**
*	@desc	減色処理	ROI内の色をinputMatで塗り潰す
*	@param	inputMat	BGRマルチチャンネルMatData
*	@param	roiSize		塗り潰す範囲の辺の長さ
*	@param	colors		色の階層数	3乗が配色数になる
*/
void ReduceRoi(cv::Mat &inputMat,int roiSize,int colors) {
	for (int y = 0; y < inputMat.size().height; y += roiSize) {
		for (int x = 0; x < inputMat.size().width; x += roiSize) {
			// ROI
			cv::Rect roi(x, y, roiSize, roiSize);
			cv::Mat roiMat = inputMat(roi);

			// ROIの平均色の算出
			// 行平均化
			cv::Mat avgRow = inputMat(roi);
			cv::reduce(roiMat, avgRow, CV_SORT_EVERY_ROW, CV_REDUCE_AVG);
			// 列平均化
			cv::Mat avgAll = avgRow.clone();
			cv::reduce(avgRow, avgAll, CV_SORT_EVERY_COLUMN, CV_REDUCE_AVG);

			// BGRチャネルに分ける
			std::vector<cv::Mat> channel;
			cv::split(avgAll, channel);

			// 減色処理
			int reduce = MAX_COLORS / colors;
			for (int i = 0; i < 3; i++) {
				channel[i].data[0] = channel[i].data[0] / reduce * reduce + reduce / 2;
			}
			// ROIを平均色で塗りつぶし
			cv::rectangle(inputMat, roi, cv::Scalar(channel[0].data[0], channel[1].data[0], channel[2].data[0]), CV_FILLED);
		}
	}
}

//void Trackbar(int value,void *userData) {
//	cv::Mat inputMat = *(static_cast<cv::Mat*>(userData));
//	cv::Mat outputMat = inputMat.clone();
//	ResizeImage(inputMat,outputMat,value);
//	ReduceRoi(outputMat,value);
//
//	cv::imshow("img",outputMat);
//}

// input picture fileName
int main(int argc, const char *argv[]) {
//int main(){
	cv::Mat beforePictureMat;
	cv::namedWindow("img");
	// 画像の読み込み
	cv::String pictureAddress(argv[1]);
	int clors = COLORS;
	if (argv[3]) { clors = (int)argv[3]; }
	beforePictureMat = cv::imread(pictureAddress,CV_LOAD_IMAGE_COLOR);
	if (beforePictureMat.data == false) {
		printf("UNKNOWN INPUT IMAGE\n");
		return -1;
	}
	//src = cv::imread(pictureAddress, CV_LOAD_IMAGE_COLOR);
	//beforePictureMat = cv::imread("C:/cvtestdata.png",CV_LOAD_IMAGE_COLOR);
	//split?
	// サイズチェック
	//int height = beforePictureMat.size().height;
	//int width = beforePictureMat.size().width;
	// 分割処理
	// トラックバー	
	int cutSizeValue = DEFAULT_DOT_SIZE;
	cv::Mat afterPictureMat = beforePictureMat.clone();
//	cv::createTrackbar("DotSize", "img", &cutSizeValue, MAX_DOT_SIZE, Trackbar, &afterPictureMat);
	cv::createTrackbar("DotSize", "img", &cutSizeValue, MAX_DOT_SIZE, NULL, NULL);

	while (true) {

		ResizeImage(beforePictureMat, afterPictureMat, cutSizeValue);

		//// リサイズ処理
		//int fixY = (height % cutSizeValue);
		//int fixX = (width % cutSizeValue);
		//if (fixY != 0) {
		//	//beforePictureMat.resize((cv::Size(width,height + fix)));
		//	// バイキュービック法でのリサイズ（重い場合変更）
		//	if (fixX != 0) {
		//		cv::resize(beforePictureMat, afterPictureMat, cv::Size(width - fixX, height - fixY), CV_INTER_CUBIC);
		//	} else {
		//		cv::resize(beforePictureMat, afterPictureMat, cv::Size(width, height - fixY), CV_INTER_CUBIC);
		//	}
		//} else if(fixX != 0) {
		//	cv::resize(beforePictureMat, afterPictureMat, cv::Size(width - fixX, height), CV_INTER_CUBIC);
		//}

		// 平均化
		ReduceRoi(afterPictureMat, cutSizeValue, clors);
		//for (int y = 0; y < afterPictureMat.size().height; y += cutSizeValue) {
		//	for (int x = 0; x < afterPictureMat.size().width; x += cutSizeValue) {
		//		// ROI
		//		cv::Rect roi(x, y, cutSizeValue, cutSizeValue);
		//		cv::Mat roiMat = afterPictureMat(roi);
		//		// ROIの平均色の算出
		//		// 行平均化
		//		cv::Mat avgRow = afterPictureMat(roi);
		//		cv::reduce(roiMat, avgRow, CV_SORT_EVERY_ROW, CV_REDUCE_AVG);
		//		// 列平均化
		//		cv::Mat avgCol = avgRow.clone();
		//		cv::reduce(avgRow, avgCol, CV_SORT_EVERY_COLUMN, CV_REDUCE_AVG);
		//		// BGRチャネルに分ける
		//		std::vector<cv::Mat> channel;
		//		cv::split(avgCol,channel);
		//		// 減色処理
		//		int reduce = MAX_COLORS / COLORS;
		//		for (int i = 0; i < 3; i++) {
		//			channel[i].data[0] =  channel[i].data[0] / reduce * reduce + reduce / 2;
		//		}
		//		// ROIを平均色で塗りつぶし
		//		cv::rectangle(afterPictureMat, roi, cv::Scalar(channel[0].data[0],channel[1].data[0],channel[2].data[0]), CV_FILLED);
		//	}
		//}
		// 画像のの出力

		cv::imshow("img",afterPictureMat);

		// ESCキーで終了 その他のキーで更新
		if (cv::waitKey(0) == 27) {
			break;
		}
		afterPictureMat = beforePictureMat.clone();
	}
	// 画像の保存

	cv::String filename(argv[2]);
	cv::imwrite(filename, afterPictureMat);

	cv::destroyAllWindows();

	return 0;

}
