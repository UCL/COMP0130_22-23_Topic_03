/**
 * This file is part of ORB-SLAM2.
 *
 * Copyright (C) 2014-2016 Raúl Mur-Artal <raulmur at unizar dot es> (University
 * of Zaragoza) For more information see <https://github.com/raulmur/ORB_SLAM2>
 *
 * ORB-SLAM2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ORB-SLAM2 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ORB-SLAM2. If not, see <http://www.gnu.org/licenses/>.
 */

#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <sysexits.h>

#include <opencv2/core/core.hpp>

#include <System.h>

using namespace std;

void LoadImages(const string &strAssociationFilename,
                vector<string> &vstrImageFilenamesRGB,
                vector<string> &vstrImageFilenamesD,
                vector<double> &vTimestamps);

int main(int argc, char **argv) {
  if (argc != 4) {
    cerr << endl
         << "Usage: " << argv[0] << " path_to_settings path_to_sequence "
            "path_to_association"
         << endl;
    return EX_USAGE;
  }

  // Retrieve paths to images
  vector<string> vstrImageFilenamesRGB;
  vector<string> vstrImageFilenamesD;
  vector<double> vTimestamps;
  string strAssociationFilename = string(argv[3]);
  LoadImages(strAssociationFilename, vstrImageFilenamesRGB, vstrImageFilenamesD,
             vTimestamps);

  // Check consistency in the number of images and depthmaps
  int nImages = vstrImageFilenamesRGB.size();
  if (vstrImageFilenamesRGB.empty()) {
    cerr << endl << "No images found in provided path." << endl;
    return EX_DATAERR;
  } else if (vstrImageFilenamesD.size() != vstrImageFilenamesRGB.size()) {
    cerr << endl << "Different number of images for rgb and depth." << endl;
    return EX_DATAERR;
  }

  // Create SLAM system. It initializes all system threads and gets ready to
  // process frames.
  string settingsFile = string(DEFAULT_RGB_SETTINGS_DIR) + string(argv[1]);
  ORB_SLAM2::System SLAM(DEFAULT_BINARY_ORB_VOCABULARY, settingsFile,
                         ORB_SLAM2::System::RGBD, true);

  // Vector for tracking time statistics
  vector<float> vTimesTrack;
  vTimesTrack.resize(nImages);

  cout << endl << "-------" << endl;
  cout << "Start processing sequence ..." << endl;
  cout << "Images in the sequence: " << nImages << endl << endl;

  int main_error = 0;
  std::thread runthread([&]() { // Start in new thread
      // Main loop
      cv::Mat imRGB, imD;
      for (int ni = 0; ni < nImages; ni++) {
        // Read image and depthmap from file
        imRGB = cv::imread(string(argv[2]) + "/" + vstrImageFilenamesRGB[ni],
                           cv::IMREAD_UNCHANGED);
        imD = cv::imread(string(argv[2]) + "/" + vstrImageFilenamesD[ni],
                         cv::IMREAD_UNCHANGED);
        double tframe = vTimestamps[ni];
        
        if (imRGB.empty()) {
          cerr << endl
               << "Failed to load image at: " << string(argv[2]) << "/"
               << vstrImageFilenamesRGB[ni] << endl;
          main_error = EX_DATAERR;
          break;
        }
        
        if (SLAM.isFinished() == true) {
	  break;
        }

        chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();

        // Pass the image to the SLAM system
        SLAM.TrackRGBD(imRGB, imD, tframe);

        chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();
        
        double ttrack =
          std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1)
          .count();
        
        vTimesTrack[ni] = ttrack;

        // Wait to load the next frame
        double T = 0;
        if (ni < nImages - 1)
          T = vTimestamps[ni + 1] - tframe;
        else if (ni > 0)
          T = tframe - vTimestamps[ni - 1];
        
        if (ttrack < T)
          this_thread::sleep_for(chrono::duration<double>(T - ttrack));
      }
      SLAM.StopViewer();
    });
  
  cout << "Viewer started, waiting for thread." << endl;
  runthread.join();
  if (main_error != 0)
    return main_error;
  cout << "Tracking thread joined..." << endl;
  
  // Stop all threads
  SLAM.Shutdown();

  // Tracking time statistics
  sort(vTimesTrack.begin(), vTimesTrack.end());
  float totaltime = 0;
  for (int ni = 0; ni < nImages; ni++) {
    totaltime += vTimesTrack[ni];
  }
  cout << "-------" << endl << endl;
  cout << "median tracking time: " << vTimesTrack[nImages / 2] << endl;
  cout << "mean tracking time: " << totaltime / nImages << endl;

  // Save camera trajectory
  SLAM.SaveTrajectoryTUM("CameraTrajectory.txt");
  SLAM.SaveKeyFrameTrajectoryTUM("KeyFrameTrajectory.txt");

  return EX_OK;
}

void LoadImages(const string &strAssociationFilename,
                vector<string> &vstrImageFilenamesRGB,
                vector<string> &vstrImageFilenamesD,
                vector<double> &vTimestamps) {
  ifstream fAssociation;
  fAssociation.open(strAssociationFilename.c_str());
  while (!fAssociation.eof()) {
    string s;
    getline(fAssociation, s);
    if (!s.empty()) {
      stringstream ss;
      ss << s;
      double t;
      string sRGB, sD;
      ss >> t;
      vTimestamps.push_back(t);
      ss >> sRGB;
      vstrImageFilenamesRGB.push_back(sRGB);
      ss >> t;
      ss >> sD;
      vstrImageFilenamesD.push_back(sD);
    }
  }
}
