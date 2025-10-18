import {mainSlice} from 'pages/model'
import {useState} from 'react'
import {Link, useNavigate} from 'react-router-dom'
import {SERVER_URL, URL_ADD} from 'shared/consts'
import {useAppDispatch, useAppSelector} from 'shared/redux'
import {DocumentType} from 'shared/types'
import {Button} from 'shared/ui/button'
import {Input} from 'shared/ui/input'
import styles from './MainPage.module.css'

const MainPage = () => {
	const navigate = useNavigate()
	const dispatch = useAppDispatch()
	const {documents} = useAppSelector(state => state.documents)
	const [search, setSearch] = useState<string>('')

	const handleSearch = async () => {
		try {
			if (!search) {
				dispatch(mainSlice.actions.setDocuments([]))
				return
			}
			const res = await fetch(SERVER_URL + '/' + search)
			if (res.ok) {
				const data: DocumentType[] = await res.json()
				dispatch(mainSlice.actions.setDocuments(data ?? []))
				setSearch('')
			}
			else {
				alert('Error: Document not found')
			}
		}
		catch (error) {
			alert('Error: ' + error)
		}
	}

	const handleRedirect = (url: string) => {
		navigate(`/${url}`)
		dispatch(mainSlice.actions.serUrl(url))
	}

	return (
		<div className={styles.main}>
			<Button onClick={() => navigate(URL_ADD)}>{'Add Document'}</Button>
			<div className={styles.search}>
				<Input
					placeholder={'Enter URL'}
					value={search}
					onChange={e => setSearch(e.target.value)}
				/>
				<Button onClick={handleSearch}>{'Search'}</Button>
			</div>
			{documents.length === 0 && <span>{'No search results'}</span>}
			{documents.length !== 0 && documents.map((document, index) => (
				<Link key={index} to={`/${document.url}`} onClick={() => handleRedirect(document.url)}>
					<span>{document.title}</span>
				</Link>
			))}
		</div>
	)
}

export {
	MainPage,
}