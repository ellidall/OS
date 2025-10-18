import {useMemo} from 'react'
import {useNavigate} from 'react-router-dom'
import {URL_MAIN} from 'shared/consts'
import {useAppSelector} from 'shared/redux'
import {Button} from 'shared/ui/button'

const DocumentPage = () => {
	const navigate = useNavigate()
	const {currentUrl, documents} = useAppSelector(state => state.documents)
	const currentDocument = useMemo(
		() => documents.find(doc => doc.url === currentUrl),
		[documents, currentUrl],
	)

	return (
		<div>
			<div>{currentDocument?.url}</div>
			<div>{currentDocument?.title}</div>
			<div>{currentDocument?.content}</div>
			<Button onClick={() => navigate(URL_MAIN)}>{'Back'}</Button>
		</div>
	)
}

export {
	DocumentPage,
}