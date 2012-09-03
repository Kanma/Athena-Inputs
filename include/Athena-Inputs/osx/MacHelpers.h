#ifndef _ATHENA_INPUTS_MACHELPERS_H_
#define _ATHENA_INPUTS_MACHELPERS_H_

#import <CoreFoundation/CoreFoundation.h>

inline CFArrayRef getDictionaryItemAsArrayRef(CFDictionaryRef dict, const char* keyName)
{
    CFStringRef cfKeyName = CFStringCreateWithCString(NULL, keyName, kCFStringEncodingUTF8);
    CFTypeRef temp = CFDictionaryGetValue(dict, cfKeyName);
    CFRelease(cfKeyName);

    if(temp && CFGetTypeID(temp) == CFArrayGetTypeID())
        return (CFArrayRef)temp;
    else
        return 0;
}


inline CFStringRef getDictionaryItemAsStringRef(CFDictionaryRef dict, const char* keyName)
{
    CFStringRef cfKeyName = CFStringCreateWithCString(NULL, keyName, kCFStringEncodingUTF8);
    CFTypeRef temp = CFDictionaryGetValue(dict, cfKeyName);
    CFRelease(cfKeyName);

    if(temp && CFGetTypeID(temp) == CFStringGetTypeID())
        return (CFStringRef)temp;
    else
        return 0;
}


inline CFNumberRef getDictionaryItemAsNumberRef(CFDictionaryRef dict, const char* keyName)
{
    CFStringRef cfKeyName = CFStringCreateWithCString(NULL, keyName, kCFStringEncodingUTF8);
    CFTypeRef temp = CFDictionaryGetValue(dict, cfKeyName);
    CFRelease(cfKeyName);

    if(temp && CFGetTypeID(temp) == CFNumberGetTypeID())
        return (CFNumberRef)temp;
    else
        return 0;
}


inline CFDictionaryRef getArrayItemAsDictionaryRef(CFArrayRef array, CFIndex idx)
{
    CFTypeRef temp = CFArrayGetValueAtIndex(array, idx);

    if(temp && CFGetTypeID(temp) == CFDictionaryGetTypeID())
        return (CFDictionaryRef)temp;
    else
        return 0;
}


inline int getInt32(CFNumberRef ref)
{
    int r;
    if (CFNumberGetValue(ref, kCFNumberIntType, &r))
        return r;

    return 0;
}

#endif
