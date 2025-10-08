#include <QApplication>
#include "TelescopeGUI.hpp"

CometObserver *global_observer;

/**
 * @brief Main function for the Celestron Origin Monitor application
 * 
 * This application provides a graphical user interface for monitoring
 * and controlling Celestron Origin telescopes. It automatically discovers
 * telescopes on the network and displays their status information.
 * 
 * @param argc Command line argument count
 * @param argv Command line arguments
 * @return Application exit code
 */
int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    CometObserver observer;
    global_observer = &observer;
    ObserverLocation myLocation;
    myLocation.latitude = 52.2053;
    myLocation.longitude = 0.1218;
    myLocation.elevation = 20.0;
    observer.setLocation(myLocation);
    QString kernelDir = "/Users/jonathan/spice_kernels/";
    QString outputDir = kernelDir; // Write output to same directory
    QString horizonsFile = kernelDir + "horizons_reference.txt";
    QString cometName = "1004054";

    if (!observer.loadKernels(
        kernelDir + "c2025a6.bsp",
        kernelDir + "naif0012.tls",
        kernelDir + "de440s.bsp"))
    {
        qDebug() << "Failed to load kernels";
        return 1;
    }
    
    // Create and show the main window
    TelescopeGUI *gui = new TelescopeGUI();
    gui->show();
    
    // Start the application event loop
    return app.exec();
}
