
/**
 * Photo Manager Application - C++ Qt Version
 *
 * Design Patterns Implemented:
 * 1. Singleton - Logger, Repositories, PhotoManagementFacade
 * 2. Builder - User construction
 * 3. Factory - AuthenticationFactory
 * 4. Repository - UserRepository, PhotoRepository
 * 5. Strategy - StorageStrategy (Local/Cloud)
 * 6. Decorator - ImageProcessor chain
 * 7. Facade - PhotoManagementFacade
 * 8. Command - UploadPhotoCommand with undo support
 * 9. Observer - Logger observing all actions
 * 10. MVC - All GUI components

 * 11. Chain of responsability -  image processing pipeline 
 * 12. Iteration pattern - Search engine
 * 13. Composite - album/folder management 
 * 14. memento - edit history, undo function 
 * 15. Proxy - lazy loading and access control 
 * 16. State - photo lifecycle management 
 * 17. Adapter - third-party integrations
 */



cpp_version/
├── CMakeLists.txt              # Build configuration
├── main.cpp                    # Application entry point
│
├── Domain Layer (Entities)
│   ├── User.h/cpp              # User entity with Builder
│   └── Photo.h/cpp             # Photo entity
│
├── Infrastructure Layer
│   ├── UserRepository.h/cpp    # User data access (Singleton)
│   ├── PhotoRepository.h/cpp   # Photo data access (Singleton)
│   ├── Logger.h/cpp            # Logging service (Singleton, Observer)
│   ├── StorageStrategy.h/cpp   # Storage abstraction (Strategy)
│   ├── AuthenticationService.h/cpp     # Auth services
│   └── AuthenticationFactory.h/cpp     # Auth factory (Factory)
│
├── Business Logic Layer
│   ├── PhotoManagementFacade.h/cpp    # Business facade (Facade)
│   ├── ImageProcessor.h/cpp           # Image processing (Decorator)
│   ├── Command.h/cpp                  # Commands (Command)
│   └── PhotoSearchCriteria.h/cpp      # Search specification
│
└── Presentation Layer (Qt GUI)
    ├── LoginWindow.h/cpp       # Login screen (MVC)
    ├── MainWindow.h/cpp        # Main application window (MVC)
    ├── UploadDialog.h/cpp      # Upload dialog
    ├── SearchDialog.h/cpp      # Search dialog
    ├── PhotoDetailsDialog.h/cpp # Photo details
    └── AdminPanel.h/cpp        # Admin panel


