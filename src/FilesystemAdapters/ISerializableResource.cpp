#include "FilesystemAdapters/ISerializableResource.h"

using filesystem_adapters::ISerializableResource;
using LockedResource = filesystem_adapters::ISerializableResource::LockedResource;

ISerializableResource::ISerializableResource() = default;
ISerializableResource::~ISerializableResource() = default;

LockedResource ISerializableResource::Lock()
{
    return LockedResource(*this);
}

const LockedResource ISerializableResource::Lock() const
{
    return LockedResource(const_cast<ISerializableResource &>(*this));
}

LockedResource::LockedResource(ISerializableResource &o) : obj_(&o), lk_(o.mtx_) {}
size_t LockedResource::GetColumnSize() const { return obj_->GetColumnSize(); };
size_t LockedResource::GetRowSize() const { return obj_->GetRowSize(); };
bool LockedResource::GetDirty() const { return obj_->GetDirty(); };
size_t LockedResource::GetElementSize() const { return obj_->GetElementSize(); };
void *LockedResource::Data() { return obj_->Data(); };
const void *LockedResource::Data() const { return obj_->Data(); };
void LockedResource::Assign(const char *buff, const size_t n) { return obj_->Assign(buff, n); };
void LockedResource::SetColumnSize(const size_t size) { obj_->SetColumnSize(size); };
void LockedResource::SetRowSize(const size_t size) { obj_->SetRowSize(size); };
bool LockedResource::UpdateChecksum() const { return obj_->UpdateChecksum(); };
int LockedResource::Checksum() const { return obj_->Checksum(); };
