#pragma once
namespace Platform {
struct DNSLookup;
enum class DNSLookupStatus { Pending, Failed, Succeeded };
DNSLookup* StartDNSLookup(const char* hostname);
DNSLookupStatus PollDNSLookup(DNSLookup*& lookup);
void CancelDNSLookup(DNSLookup*& lookup);
}
