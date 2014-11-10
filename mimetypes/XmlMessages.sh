function get_files
{
    echo krecipes-mime.xml
}

function po_for_file
{
    case "$1" in
       krecipes-mime.xml)
           echo krecipes_xml_mimetypes.po
       ;;
    esac
}

function tags_for_file
{
    case "$1" in
       krecipes-mime.xml)
           echo comment
       ;;
    esac
}

