#!/bin/bash

paths=(
  /usr/local/include/EntitiesModule
  /usr/local/include/Config
  /usr/local/include/test_intraprocess
  /usr/local/include/Resources
  /usr/local/include/Interprocess
  /usr/local/include/test_filesystem_adapters
  /usr/local/include/test_entities
  /usr/local/include/test_events
  /usr/local/include/DatabaseAdapters
  /usr/local/include/test_database_adapters
  /usr/local/include/test_resources
  /usr/local/include/Events
  /usr/local/include/FilesystemAdapters
  /usr/local/include/Intraprocess
  /usr/local/include/Entities
  /usr/local/include/ResourcesModule
  /usr/local/share/X/XConfigVersion.cmake
  /usr/local/lib/libDatabaseAdapters.*
  /usr/local/share/DatabaseAdapters
  /usr/local/lib/libEntities.*
  /usr/local/share/Entities
  /usr/local/lib/libEvents.*
  /usr/local/share/Events
  /usr/local/lib/libFilesystemAdapters.*
  /usr/local/share/FilesystemAdapters
  /usr/local/lib/libInterprocess.*
  /usr/local/share/Interprocess
  /usr/local/lib/libIntraprocess.*
  /usr/local/share/Intraprocess
  /usr/local/lib/libResources.*
  /usr/local/share/Resources
)

echo "Removing installed files and directories..."

for path in "${paths[@]}"; do
  if [ -e "$path" ]; then
    echo "Removing: $path"
    sudo rm -rf "$path"
  else
    echo "Not found: $path"
  fi
done

echo "Done."
