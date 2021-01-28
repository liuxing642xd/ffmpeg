# DO NOT EDIT
# This makefile makes sure all linkable targets are
# up-to-date with anything they link to
default:
	echo "Do not invoke directly"

# Rules to remove targets that are older than anything to which they
# link.  This forces Xcode to relink the targets from scratch.  It
# does not seem to check these dependencies itself.
PostBuild.Jnd.Debug:
/Users/chenfeifei/DeepLearning/algorithm/AgoraAI/build/ios/lib/Debug/libJnd.a:
	/bin/rm -f /Users/chenfeifei/DeepLearning/algorithm/AgoraAI/build/ios/lib/Debug/libJnd.a
	/bin/rm -f /Users/chenfeifei/DeepLearning/algorithm/AgoraAI/build/ios/libJnd/Jnd.build/Debug/Jnd.build/Objects-normal/armv7/libJnd.a
	/bin/rm -f /Users/chenfeifei/DeepLearning/algorithm/AgoraAI/build/ios/libJnd/Jnd.build/Debug/Jnd.build/Objects-normal/arm64/libJnd.a


PostBuild.Jnd.Release:
/Users/chenfeifei/DeepLearning/algorithm/AgoraAI/build/ios/lib/Release/libJnd.a:
	/bin/rm -f /Users/chenfeifei/DeepLearning/algorithm/AgoraAI/build/ios/lib/Release/libJnd.a
	/bin/rm -f /Users/chenfeifei/DeepLearning/algorithm/AgoraAI/build/ios/libJnd/Jnd.build/Release/Jnd.build/Objects-normal/armv7/libJnd.a
	/bin/rm -f /Users/chenfeifei/DeepLearning/algorithm/AgoraAI/build/ios/libJnd/Jnd.build/Release/Jnd.build/Objects-normal/arm64/libJnd.a




# For each target create a dummy ruleso the target does not have to exist
