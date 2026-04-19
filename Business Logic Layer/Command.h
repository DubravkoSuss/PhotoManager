#ifndef COMMAND_H
#define COMMAND_H

#include "PhotoManagementFacade.h"
#include <QStack>
#include <memory>

/**
 * Command Pattern - Encapsulates user actions
 */
class Command {
public:
    virtual ~Command() = default;
    virtual void execute() = 0;
    virtual void undo() = 0;
};

/**
 * Upload Photo Command
 */

class UploadPhotoCommand : public Command {
public:
    UploadPhotoCommand(PhotoManagementFacade* facade, std::shared_ptr<User> user,
                      const QString& filePath, const QString& description,
                      const QStringList& hashtags, std::shared_ptr<ImageProcessor> processor)
        : facade_(facade), user_(user), filePath_(filePath),
          description_(description), hashtags_(hashtags), processor_(processor) {}

    void execute() override {
        uploadedPhoto_ = facade_->uploadPhoto(user_, filePath_, description_, hashtags_, processor_);
    }

    void undo() override {
        if (uploadedPhoto_) {
            facade_->deletePhotos(user_, uploadedPhoto_->getPhotoId());
        }
    }

private:
    PhotoManagementFacade* facade_;
    std::shared_ptr<User> user_;
    QString filePath_;
    QString description_;
    QStringList hashtags_;
    std::shared_ptr<ImageProcessor> processor_;
    std::shared_ptr<Photo> uploadedPhoto_;
};

/**
 * Command Invoker
 */
class CommandInvoker {
public:
    void executeCommand(std::shared_ptr<Command> command) {
        command->execute();
        commandHistory_.push(command);
    }

    void undoLastCommand() {
        if (!commandHistory_.isEmpty()) {
            auto command = commandHistory_.pop();
            command->undo();
        }
    }

private:
    QStack<std::shared_ptr<Command>> commandHistory_;
};

#endif // COMMAND_H