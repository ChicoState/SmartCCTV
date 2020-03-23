/*
  File Name:  exFaceDet.go
  Created By:  GoCV/Connor Adams - Modified GoCV base code
  Created On:  3/23/20

  Description: Example Golang code of a GoCV implementation of their OpenCV interface that I've modified to be a little bit more relevant
               to our needs. Uploaded at behest of team, this server as a simple example of Golang's approach to similar problem with the
               OpenCV interface available. NOTE: Uses a Haar cascade file (.xml) to classify front-facing human faces. 

  Usage: "go run exFaceDet.go deviceID classifierFile blurOption" 
*/

package main

import (
    "fmt"
    "image"
    "image/color"
    "os"
    "strconv"

    "gocv.io/x/gocv"
)

func takePic(fName, devID string) error {
    // Open device 'devID'
    cam, err := gocv.OpenVideoCapture(devID)

    // Get image from cam
    img := gocv.NewMat()
    cam.Read(&img)

    // Write image and close resources
    gocv.IMWrite(fName, img)

    // Return conditions
    img.Close()
    cam.Close()
    return err
}

func main() {
    // parse args
    deviceID, _ := strconv.Atoi(os.Args[1])
    xmlFile := os.Args[2]
    blurOpt, _ := strconv.Atoi(os.Args[3])

    // open webcam
    webcam, _ := gocv.VideoCaptureDevice(int(deviceID))
    defer webcam.Close()

    // open display window
    window := gocv.NewWindow("Face Detect")
    defer window.Close()

    // prepare image matrix
    img := gocv.NewMat()
    defer img.Close()

    // load classifier to recognize faces
    classifier := gocv.NewCascadeClassifier()
    defer classifier.Close()

    if !classifier.Load(xmlFile) {
        fmt.Printf("Error reading cascade file: %v\n", xmlFile)
        return
    }
    
    fmt.Printf("start reading camera device: %v\n", deviceID)
    for {
        if ok := webcam.Read(&img); !ok {
            fmt.Printf("cannot read device %d\n", deviceID)
            return
        }
        
        // pass thru if no image detected from device '(deviceID)'
        if img.Empty() {
            fmt.Printf("empty image on device %d...continuing...\n", deviceID)
            continue
        }

        // detect faces
        rects := classifier.DetectMultiScale(img)
        if len(rects) > 0 {
            fmt.Printf("%d face(s) found...\n", len(rects))
        }
        
        for _, r := range rects {
            // draw rectangle
            gocv.Rectangle(&img, r, color.RGBA{0, 255, 0, 0}, 3)

            // define face and blur region
            if blurOpt == 1 {
                imgFace := img.Region(r)
                gocv.GaussianBlur(imgFace, &imgFace, image.Pt(75, 75), 0, 0, gocv.BorderDefault)
            }
            
            // classify roi & label
            size := gocv.GetTextSize("Human - Face", gocv.FontHersheyPlain, 1.2, 2)
            pt := image.Pt(r.Min.X+(r.Min.X/2)-(size.X/2), r.Min.Y-2)
            gocv.PutText(&img, "Human - Face", pt, gocv.FontHersheyPlain, 1.2, color.RGBA{0, 255, 0, 0}, 2)
        }

        // show the image in the window, and wait 1 millisecond
        window.IMShow(img)
        if window.WaitKey(1) >= 0 {
            break
        }
    }
}
