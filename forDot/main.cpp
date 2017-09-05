#include <opencv2\core\core.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\highgui\highgui.hpp>

// ����DOT�T�C�Y
#define DEFAULT_DOT_SIZE 5
#define MAX_DOT_SIZE 40
// �F�K�����@COLORS�̎O�悪�F��
#define COLORS 4
#define MAX_COLORS 256

// ���T�C�Y����
void ResizeImage(cv::Mat &inputMat,cv::Mat &outputMat,int dotSize = DEFAULT_DOT_SIZE) {
	int fixY = (inputMat.size().height % dotSize);
	int fixX = (inputMat.size().width % dotSize);
	if (fixY != 0) {
		// �o�C�L���[�r�b�N�@�ł̃��T�C�Y
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
*	@desc	���F����	ROI���̐F��inputMat�œh��ׂ�
*	@param	inputMat	BGR�}���`�`�����l��MatData
*	@param	roiSize		�h��ׂ��͈͂̕ӂ̒���
*	@param	colors		�F�̊K�w��	3�悪�z�F���ɂȂ�
*/
void ReduceRoi(cv::Mat &inputMat,int roiSize,int colors) {
	for (int y = 0; y < inputMat.size().height; y += roiSize) {
		for (int x = 0; x < inputMat.size().width; x += roiSize) {
			// ROI
			cv::Rect roi(x, y, roiSize, roiSize);
			cv::Mat roiMat = inputMat(roi);

			// ROI�̕��ϐF�̎Z�o
			// �s���ω�
			cv::Mat avgRow = inputMat(roi);
			cv::reduce(roiMat, avgRow, CV_SORT_EVERY_ROW, CV_REDUCE_AVG);
			// �񕽋ω�
			cv::Mat avgAll = avgRow.clone();
			cv::reduce(avgRow, avgAll, CV_SORT_EVERY_COLUMN, CV_REDUCE_AVG);

			// BGR�`���l���ɕ�����
			std::vector<cv::Mat> channel;
			cv::split(avgAll, channel);

			// ���F����
			int reduce = MAX_COLORS / colors;
			for (int i = 0; i < 3; i++) {
				channel[i].data[0] = channel[i].data[0] / reduce * reduce + reduce / 2;
			}
			// ROI�𕽋ϐF�œh��Ԃ�
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
	// �摜�̓ǂݍ���
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
	// �T�C�Y�`�F�b�N
	//int height = beforePictureMat.size().height;
	//int width = beforePictureMat.size().width;
	// ��������
	// �g���b�N�o�[	
	int cutSizeValue = DEFAULT_DOT_SIZE;
	cv::Mat afterPictureMat = beforePictureMat.clone();
//	cv::createTrackbar("DotSize", "img", &cutSizeValue, MAX_DOT_SIZE, Trackbar, &afterPictureMat);
	cv::createTrackbar("DotSize", "img", &cutSizeValue, MAX_DOT_SIZE, NULL, NULL);

	while (true) {

		ResizeImage(beforePictureMat, afterPictureMat, cutSizeValue);

		//// ���T�C�Y����
		//int fixY = (height % cutSizeValue);
		//int fixX = (width % cutSizeValue);
		//if (fixY != 0) {
		//	//beforePictureMat.resize((cv::Size(width,height + fix)));
		//	// �o�C�L���[�r�b�N�@�ł̃��T�C�Y�i�d���ꍇ�ύX�j
		//	if (fixX != 0) {
		//		cv::resize(beforePictureMat, afterPictureMat, cv::Size(width - fixX, height - fixY), CV_INTER_CUBIC);
		//	} else {
		//		cv::resize(beforePictureMat, afterPictureMat, cv::Size(width, height - fixY), CV_INTER_CUBIC);
		//	}
		//} else if(fixX != 0) {
		//	cv::resize(beforePictureMat, afterPictureMat, cv::Size(width - fixX, height), CV_INTER_CUBIC);
		//}

		// ���ω�
		ReduceRoi(afterPictureMat, cutSizeValue, clors);
		//for (int y = 0; y < afterPictureMat.size().height; y += cutSizeValue) {
		//	for (int x = 0; x < afterPictureMat.size().width; x += cutSizeValue) {
		//		// ROI
		//		cv::Rect roi(x, y, cutSizeValue, cutSizeValue);
		//		cv::Mat roiMat = afterPictureMat(roi);
		//		// ROI�̕��ϐF�̎Z�o
		//		// �s���ω�
		//		cv::Mat avgRow = afterPictureMat(roi);
		//		cv::reduce(roiMat, avgRow, CV_SORT_EVERY_ROW, CV_REDUCE_AVG);
		//		// �񕽋ω�
		//		cv::Mat avgCol = avgRow.clone();
		//		cv::reduce(avgRow, avgCol, CV_SORT_EVERY_COLUMN, CV_REDUCE_AVG);
		//		// BGR�`���l���ɕ�����
		//		std::vector<cv::Mat> channel;
		//		cv::split(avgCol,channel);
		//		// ���F����
		//		int reduce = MAX_COLORS / COLORS;
		//		for (int i = 0; i < 3; i++) {
		//			channel[i].data[0] =  channel[i].data[0] / reduce * reduce + reduce / 2;
		//		}
		//		// ROI�𕽋ϐF�œh��Ԃ�
		//		cv::rectangle(afterPictureMat, roi, cv::Scalar(channel[0].data[0],channel[1].data[0],channel[2].data[0]), CV_FILLED);
		//	}
		//}
		// �摜�̂̏o��

		cv::imshow("img",afterPictureMat);

		// ESC�L�[�ŏI�� ���̑��̃L�[�ōX�V
		if (cv::waitKey(0) == 27) {
			break;
		}
		afterPictureMat = beforePictureMat.clone();
	}
	// �摜�̕ۑ�

	cv::String filename(argv[2]);
	cv::imwrite(filename, afterPictureMat);

	cv::destroyAllWindows();

	return 0;

}
